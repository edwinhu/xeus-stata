#include "xeus-stata/xinterpreter.hpp"
#include "xeus-stata/stata_session.hpp"
#include "xeus-stata/completion.hpp"
#include "xeus-stata/inspection.hpp"
#include "xeus-stata/xeus_stata_config.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

namespace xeus_stata
{
    interpreter::interpreter()
        : m_session(nullptr)
        , m_completer(nullptr)
        , m_inspector(nullptr)
    {
    }

    interpreter::~interpreter()
    {
    }

    void interpreter::configure_impl()
    {
        // Initialize Stata session
        try
        {
            m_session = std::make_unique<stata_session>();
            m_completer = std::make_unique<completion_engine>(m_session.get());
            m_inspector = std::make_unique<inspection_engine>(m_session.get());
        }
        catch (const std::exception& e)
        {
            std::cerr << "Failed to initialize Stata session: " << e.what() << std::endl;
            throw;
        }
    }

    nl::json interpreter::execute_request_impl(
        int execution_counter,
        const std::string& code,
        bool silent,
        bool store_history,
        nl::json user_expressions,
        bool allow_stdin)
    {
        nl::json result;

        if (!m_session || !m_session->is_ready())
        {
            result["status"] = "error";
            result["ename"] = "RuntimeError";
            result["evalue"] = "Stata session not initialized";
            result["traceback"] = nl::json::array({"Stata session not initialized"});
            return result;
        }

        try
        {
            // Execute the code
            auto exec_result = m_session->execute(code);

            if (exec_result.is_error)
            {
                // Execution resulted in an error
                result["status"] = "error";
                result["ename"] = "StataError";
                result["evalue"] = "r(" + std::to_string(exec_result.error_code) + ")";

                nl::json traceback = nl::json::array();
                if (!exec_result.error_message.empty())
                {
                    traceback.push_back(exec_result.error_message);
                }
                traceback.push_back("Stata error code: r(" +
                                  std::to_string(exec_result.error_code) + ")");
                result["traceback"] = traceback;

                // Publish error output
                if (!silent)
                {
                    publish_stream("stderr", exec_result.error_message);
                }
            }
            else
            {
                // Success
                result["status"] = "ok";
                result["execution_count"] = execution_counter;
                result["payload"] = nl::json::array();
                result["user_expressions"] = nl::json::object();

                // Publish output
                if (!silent && !exec_result.output.empty())
                {
                    publish_stream("stdout", exec_result.output);
                }

                // Handle graphs
                for (const auto& graph_file : exec_result.graph_files)
                {
                    // Read graph file and publish as display data
                    std::ifstream file(graph_file, std::ios::binary);
                    if (file)
                    {
                        std::ostringstream ss;
                        ss << file.rdbuf();
                        std::string graph_data = ss.str();

                        // Determine MIME type based on extension
                        std::string mime_type = "image/png";
                        if (graph_file.find(".svg") != std::string::npos)
                        {
                            mime_type = "image/svg+xml";
                        }
                        else if (graph_file.find(".pdf") != std::string::npos)
                        {
                            mime_type = "application/pdf";
                        }

                        // Create display data
                        nl::json display_data;
                        display_data[mime_type] = graph_data;

                        nl::json metadata;
                        publish_execution_result(
                            execution_counter,
                            std::move(display_data),
                            std::move(metadata)
                        );
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            result["status"] = "error";
            result["ename"] = "RuntimeError";
            result["evalue"] = e.what();
            result["traceback"] = nl::json::array({e.what()});

            if (!silent)
            {
                publish_stream("stderr", std::string("Error: ") + e.what());
            }
        }

        return result;
    }

    nl::json interpreter::complete_request_impl(
        const std::string& code,
        int cursor_pos)
    {
        nl::json result;

        if (!m_completer)
        {
            result["status"] = "ok";
            result["matches"] = nl::json::array();
            result["cursor_start"] = cursor_pos;
            result["cursor_end"] = cursor_pos;
            result["metadata"] = nl::json::object();
            return result;
        }

        try
        {
            int start_pos = 0;
            auto completions = m_completer->get_completions(code, cursor_pos, start_pos);

            result["status"] = "ok";
            result["matches"] = completions;
            result["cursor_start"] = start_pos;
            result["cursor_end"] = cursor_pos;
            result["metadata"] = nl::json::object();
        }
        catch (const std::exception& e)
        {
            result["status"] = "ok";
            result["matches"] = nl::json::array();
            result["cursor_start"] = cursor_pos;
            result["cursor_end"] = cursor_pos;
            result["metadata"] = nl::json::object();
        }

        return result;
    }

    nl::json interpreter::inspect_request_impl(
        const std::string& code,
        int cursor_pos,
        int detail_level)
    {
        nl::json result;

        if (!m_inspector)
        {
            result["status"] = "ok";
            result["found"] = false;
            result["data"] = nl::json::object();
            result["metadata"] = nl::json::object();
            return result;
        }

        try
        {
            std::string help_text = m_inspector->get_inspection(code, cursor_pos, detail_level);

            if (!help_text.empty())
            {
                result["status"] = "ok";
                result["found"] = true;

                nl::json data;
                data["text/plain"] = help_text;
                result["data"] = data;
                result["metadata"] = nl::json::object();
            }
            else
            {
                result["status"] = "ok";
                result["found"] = false;
                result["data"] = nl::json::object();
                result["metadata"] = nl::json::object();
            }
        }
        catch (const std::exception&)
        {
            result["status"] = "ok";
            result["found"] = false;
            result["data"] = nl::json::object();
            result["metadata"] = nl::json::object();
        }

        return result;
    }

    nl::json interpreter::is_complete_request_impl(const std::string& code)
    {
        nl::json result;

        // Simple heuristic: check for unclosed braces or continuation markers
        int brace_count = 0;
        bool has_continuation = false;

        for (size_t i = 0; i < code.length(); ++i)
        {
            if (code[i] == '{')
                ++brace_count;
            else if (code[i] == '}')
                --brace_count;

            // Check for line continuation marker (///)
            if (i + 2 < code.length() &&
                code[i] == '/' && code[i+1] == '/' && code[i+2] == '/')
            {
                has_continuation = true;
            }
        }

        if (brace_count > 0 || has_continuation)
        {
            result["status"] = "incomplete";
            result["indent"] = "    ";
        }
        else if (brace_count < 0)
        {
            result["status"] = "invalid";
            result["indent"] = "";
        }
        else
        {
            result["status"] = "complete";
            result["indent"] = "";
        }

        return result;
    }

    nl::json interpreter::kernel_info_request_impl()
    {
        nl::json info;

        info["protocol_version"] = "5.3";
        info["implementation"] = "xeus-stata";
        info["implementation_version"] = XEUS_STATA_VERSION;

        // Language info
        info["language_info"]["name"] = "stata";
        info["language_info"]["version"] = m_session ? m_session->get_version() : "Unknown";
        info["language_info"]["mimetype"] = "text/x-stata";
        info["language_info"]["file_extension"] = ".do";
        info["language_info"]["pygments_lexer"] = "stata";
        info["language_info"]["codemirror_mode"] = "stata";

        // Banner
        std::ostringstream banner;
        banner << "xeus-stata " << XEUS_STATA_VERSION << "\n";
        banner << "A Jupyter kernel for Stata\n";
        if (m_session)
        {
            banner << "Stata version: " << m_session->get_version();
        }
        info["banner"] = banner.str();

        // Help links
        info["help_links"] = nl::json::array({
            {
                {"text", "Stata Documentation"},
                {"url", "https://www.stata.com/features/documentation/"}
            },
            {
                {"text", "xeus-stata Repository"},
                {"url", "https://github.com/jupyter-xeus/xeus-stata"}
            }
        });

        info["status"] = "ok";

        return info;
    }

    void interpreter::shutdown_request_impl()
    {
        if (m_session)
        {
            m_session->shutdown();
        }
    }

} // namespace xeus_stata
