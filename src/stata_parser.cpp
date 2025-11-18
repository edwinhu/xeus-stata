#include "xeus-stata/stata_parser.hpp"

#include <regex>
#include <sstream>
#include <iomanip>
#include <random>

namespace xeus_stata
{
    std::string generate_execution_marker()
    {
        // Generate a random hex string to use as a marker
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);

        std::stringstream ss;
        for (int i = 0; i < 16; ++i)
        {
            ss << std::hex << dis(gen);
        }
        return ss.str();
    }

    std::string strip_ansi_codes(const std::string& text)
    {
        // Remove ANSI escape sequences
        std::regex ansi_pattern("\033\\[[0-9;]*[a-zA-Z]");
        return std::regex_replace(text, ansi_pattern, "");
    }

    bool contains_error(const std::string& output, int& error_code)
    {
        // Check for Stata error pattern: r(###);
        std::regex error_pattern("r\\((\\d+)\\);");
        std::smatch match;

        if (std::regex_search(output, match, error_pattern))
        {
            error_code = std::stoi(match[1].str());
            return true;
        }

        return false;
    }

    std::vector<std::string> extract_graph_files(const std::string& output)
    {
        std::vector<std::string> graph_files;

        // Pattern: (file filename.ext written/saved...)
        std::regex graph_pattern("\\(file ([^\\s]+\\.(png|svg|pdf|eps)) (written|saved)");
        std::sregex_iterator iter(output.begin(), output.end(), graph_pattern);
        std::sregex_iterator end;

        for (; iter != end; ++iter)
        {
            graph_files.push_back((*iter)[1].str());
        }

        return graph_files;
    }

    bool is_stata_table(const std::string& output)
    {
        if (output.empty())
        {
            return false;
        }

        // Count table indicators
        int dash_lines = 0;
        int pipe_lines = 0;
        int multi_space_lines = 0;
        int total_lines = 0;

        // Check for common table keywords
        bool has_table_keywords = false;
        std::vector<std::string> keywords = {
            "Variable", "Obs", "Mean", "Std. Dev.", "Std. Err.",
            "Coef.", "P>|t|", "P>|z|", "[95% Conf. Interval]",
            "Min", "Max", "Sum", "Variance", "Skewness", "Kurtosis",
            "Number of obs", "F(", "Prob > F", "R-squared",
            "Adj R-squared", "Root MSE"
        };

        for (const auto& keyword : keywords)
        {
            if (output.find(keyword) != std::string::npos)
            {
                has_table_keywords = true;
                break;
            }
        }

        // Parse lines to detect table structure
        std::stringstream ss(output);
        std::string line;

        while (std::getline(ss, line))
        {
            total_lines++;

            // Check for lines with multiple dashes (table borders)
            if (line.find("----") != std::string::npos ||
                line.find("━━━━") != std::string::npos)
            {
                dash_lines++;
            }

            // Check for lines with pipes (table cells)
            if (line.find("|") != std::string::npos)
            {
                pipe_lines++;
            }

            // Check for lines with multiple consecutive spaces (column alignment)
            std::regex multi_space("  +");
            if (std::regex_search(line, multi_space))
            {
                multi_space_lines++;
            }
        }

        // Conservative detection: require multiple indicators
        bool has_structure = (dash_lines >= 1 || pipe_lines >= 2);
        bool has_alignment = (multi_space_lines >= 3);

        return (has_table_keywords && (has_structure || has_alignment)) ||
               (dash_lines >= 2 && multi_space_lines >= 3);
    }

    std::string format_as_html_table(const std::string& output)
    {
        // Simple approach: wrap in <pre> with CSS styling
        std::stringstream html;

        html << "<style>\n";
        html << ".stata-output {\n";
        html << "  font-family: ui-monospace, 'Cascadia Code', 'Source Code Pro', Menlo, 'DejaVu Sans Mono', Consolas, monospace;\n";
        html << "  font-size: 12px;\n";
        html << "  font-variant-ligatures: none;\n";
        html << "  background-color: #f8f8f8;\n";
        html << "  padding: 10px;\n";
        html << "  border: 1px solid #e0e0e0;\n";
        html << "  border-radius: 3px;\n";
        html << "  overflow-x: auto;\n";
        html << "  margin: 0;\n";
        html << "  line-height: 1.4;\n";
        html << "}\n";
        html << "</style>\n";
        html << "<pre class=\"stata-output\">";

        // Escape HTML special characters and convert spaces to nbsp for perfect alignment
        std::string escaped = output;

        // First escape & to avoid double-escaping
        size_t pos = 0;
        while ((pos = escaped.find("&", pos)) != std::string::npos)
        {
            escaped.replace(pos, 1, "&amp;");
            pos += 5;
        }

        // Then escape < and >
        pos = 0;
        while ((pos = escaped.find("<", pos)) != std::string::npos)
        {
            escaped.replace(pos, 1, "&lt;");
            pos += 4;
        }
        pos = 0;
        while ((pos = escaped.find(">", pos)) != std::string::npos)
        {
            escaped.replace(pos, 1, "&gt;");
            pos += 4;
        }

        html << escaped;
        html << "</pre>";

        return html.str();
    }

    execution_result parse_execution_output(const std::string& output)
    {
        execution_result result;

        // Strip ANSI codes first
        std::string cleaned = strip_ansi_codes(output);

        // Check for interrupted execution (--Break--)
        bool was_interrupted = (cleaned.find("--Break--") != std::string::npos);

        // Remove execution markers
        std::regex marker_pattern("__MARKER__[a-f0-9]+__");
        cleaned = std::regex_replace(cleaned, marker_pattern, "");

        // Remove Stata prompts and command echo
        // Pattern: ". <command>" at the start of lines
        std::regex prompt_pattern("^\\. .*$", std::regex_constants::multiline);
        cleaned = std::regex_replace(cleaned, prompt_pattern, "");

        // Remove incomplete display statements (from marker command)
        std::regex display_pattern("^\\. display \".*$", std::regex_constants::multiline);
        cleaned = std::regex_replace(cleaned, display_pattern, "");

        // Remove standalone quote marks (artifacts from marker command)
        std::regex quote_pattern("^\"\\s*$", std::regex_constants::multiline);
        cleaned = std::regex_replace(cleaned, quote_pattern, "");

        // Remove graph export wrapper lines
        std::regex quietly_patterns("^quietly (capture graph describe Graph|graph drop _all|graph export \"[^\"]+\", replace)\\s*$", std::regex_constants::multiline);
        cleaned = std::regex_replace(cleaned, quietly_patterns, "");

        std::regex if_rc_pattern("^if \\(_rc == 0\\) \\{\\s*$", std::regex_constants::multiline);
        cleaned = std::regex_replace(cleaned, if_rc_pattern, "");

        std::regex closing_brace_pattern("^\\}\\s*$", std::regex_constants::multiline);
        cleaned = std::regex_replace(cleaned, closing_brace_pattern, "");

        // Check for errors
        int error_code = 0;
        result.is_error = contains_error(cleaned, error_code);
        result.error_code = error_code;

        if (result.is_error)
        {
            // Extract error message (text before r(###);)
            size_t error_pos = cleaned.find("r(" + std::to_string(error_code) + ");");
            if (error_pos != std::string::npos)
            {
                result.error_message = cleaned.substr(0, error_pos);
                // Trim trailing whitespace
                result.error_message.erase(
                    result.error_message.find_last_not_of(" \t\n\r") + 1
                );
            }
            else
            {
                result.error_message = cleaned;
            }
        }
        else if (was_interrupted)
        {
            // Mark as error for interrupted execution
            result.is_error = true;
            result.error_code = 1;  // r(1) is the standard Stata code for user break
            result.error_message = "Execution interrupted by user";
        }

        // Extract graph files
        result.graph_files = extract_graph_files(cleaned);

        // Store cleaned output
        result.output = cleaned;

        // Remove empty lines and trim trailing whitespace only
        // IMPORTANT: Keep leading spaces for table alignment!
        std::stringstream ss(result.output);
        std::stringstream output_ss;
        std::string line;
        bool first_line = true;

        while (std::getline(ss, line))
        {
            // Only trim trailing whitespace (keep leading spaces for alignment)
            line.erase(line.find_last_not_of(" \t\r") + 1);

            // Skip empty lines
            if (!line.empty())
            {
                if (!first_line)
                {
                    output_ss << "\n";
                }
                output_ss << line;
                first_line = false;
            }
        }

        result.output = output_ss.str();

        return result;
    }

} // namespace xeus_stata
