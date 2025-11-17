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

    execution_result parse_execution_output(const std::string& output)
    {
        execution_result result;

        // Strip ANSI codes first
        std::string cleaned = strip_ansi_codes(output);

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

        // Extract graph files
        result.graph_files = extract_graph_files(cleaned);

        // Store cleaned output
        result.output = cleaned;

        // Remove empty lines and trim
        std::stringstream ss(result.output);
        std::stringstream output_ss;
        std::string line;
        bool first_line = true;

        while (std::getline(ss, line))
        {
            // Trim line
            line.erase(0, line.find_first_not_of(" \t\r"));
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
