#include "xeus-stata/inspection.hpp"
#include "xeus-stata/stata_session.hpp"

#include <sstream>

namespace xeus_stata
{
    inspection_engine::inspection_engine(stata_session* session)
        : m_session(session)
    {
    }

    std::string inspection_engine::get_inspection(
        const std::string& code,
        int cursor_pos,
        int detail_level)
    {
        // Extract word at cursor position
        int word_start = cursor_pos;
        int word_end = cursor_pos;

        // Move back to start of word
        while (word_start > 0 &&
               (std::isalnum(code[word_start - 1]) || code[word_start - 1] == '_'))
        {
            --word_start;
        }

        // Move forward to end of word
        while (word_end < static_cast<int>(code.length()) &&
               (std::isalnum(code[word_end]) || code[word_end] == '_'))
        {
            ++word_end;
        }

        std::string word = code.substr(word_start, word_end - word_start);

        if (word.empty())
        {
            return "";
        }

        // Try to get help for the word (assuming it's a command)
        return get_command_help(word);
    }

    std::string inspection_engine::get_command_help(const std::string& command)
    {
        if (!m_session || !m_session->is_ready())
        {
            return "Stata session not available";
        }

        try
        {
            // Execute "help command"
            auto result = m_session->execute("help " + command);

            if (!result.is_error)
            {
                return result.output;
            }
            else
            {
                return "No help available for '" + command + "'";
            }
        }
        catch (const std::exception& e)
        {
            return std::string("Error getting help: ") + e.what();
        }
    }

    std::string inspection_engine::get_variable_info(const std::string& variable)
    {
        if (!m_session || !m_session->is_ready())
        {
            return "";
        }

        try
        {
            // Use describe command to get variable info
            auto result = m_session->execute("describe " + variable);

            if (!result.is_error)
            {
                return result.output;
            }
        }
        catch (...)
        {
            // Ignore errors
        }

        return "";
    }

} // namespace xeus_stata
