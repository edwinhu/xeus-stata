#include "xeus-stata/completion.hpp"
#include "xeus-stata/stata_session.hpp"

#include <algorithm>
#include <cctype>

namespace xeus_stata
{
    // Basic Stata commands for completion
    static const std::vector<std::string> STATA_COMMANDS = {
        "append", "assert", "bysort", "capture", "cd", "clear", "collapse",
        "compress", "count", "describe", "display", "drop", "duplicates",
        "edit", "egen", "encode", "exit", "export", "file", "foreach",
        "format", "forvalues", "generate", "graph", "help", "histogram",
        "if", "import", "infile", "insheet", "keep", "label", "list",
        "log", "logit", "merge", "mkdir", "preserve", "quietly", "regress",
        "rename", "replace", "reshape", "restore", "return", "save",
        "scatter", "sort", "summarize", "sysuse", "tabulate", "twoway",
        "use", "while", "xi"
    };

    completion_engine::completion_engine(stata_session* session)
        : m_session(session)
    {
    }

    std::vector<std::string> completion_engine::get_completions(
        const std::string& code,
        int cursor_pos,
        int& start_pos)
    {
        // Extract the word at cursor position
        int word_start = cursor_pos;
        while (word_start > 0 &&
               (std::isalnum(code[word_start - 1]) || code[word_start - 1] == '_'))
        {
            --word_start;
        }

        std::string prefix = code.substr(word_start, cursor_pos - word_start);
        start_pos = word_start;

        // Determine context
        // For now, we'll do simple command completion
        // Future: detect if we're in a variable context, macro context, etc.

        std::vector<std::string> completions;

        // Try command completions first
        auto cmd_completions = get_command_completions(prefix);
        completions.insert(completions.end(),
                          cmd_completions.begin(),
                          cmd_completions.end());

        // Could add variable completions here if session is active
        // auto var_completions = get_variable_completions(prefix);

        return completions;
    }

    std::vector<std::string> completion_engine::get_command_completions(
        const std::string& prefix)
    {
        std::vector<std::string> completions;

        for (const auto& cmd : STATA_COMMANDS)
        {
            if (cmd.find(prefix) == 0) // starts with prefix
            {
                completions.push_back(cmd);
            }
        }

        return completions;
    }

    std::vector<std::string> completion_engine::get_variable_completions(
        const std::string& prefix)
    {
        std::vector<std::string> completions;

        if (m_session && m_session->is_ready())
        {
            // Execute "quietly ds" to get variable list
            // This is a simplified version - full implementation would
            // parse the ds output properly
            try
            {
                auto result = m_session->execute("quietly ds " + prefix + "*");
                if (!result.is_error)
                {
                    // Parse variable names from output
                    // TODO: Implement proper parsing
                }
            }
            catch (...)
            {
                // Ignore errors in completion
            }
        }

        return completions;
    }

    std::vector<std::string> completion_engine::get_function_completions(
        const std::string& prefix)
    {
        // TODO: Add Stata function completions
        return {};
    }

    std::vector<std::string> completion_engine::get_macro_completions(
        const std::string& prefix)
    {
        // TODO: Add macro completions
        return {};
    }

} // namespace xeus_stata
