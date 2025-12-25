#ifndef XEUS_STATA_PARSER_HPP
#define XEUS_STATA_PARSER_HPP

#include <string>
#include <vector>
#include "stata_session.hpp"

namespace xeus_stata
{
    // Parse Stata output to extract execution results
    execution_result parse_execution_output(const std::string& output);

    // Generate a unique execution marker
    std::string generate_execution_marker();

    // Strip ANSI escape sequences from output
    std::string strip_ansi_codes(const std::string& text);

    // Check if output contains an error
    bool contains_error(const std::string& output, int& error_code);

    // Extract graph files from output
    std::vector<std::string> extract_graph_files(const std::string& output);

    // Check if output looks like a Stata table
    bool is_stata_table(const std::string& output);

    // Format output as HTML table
    std::string format_as_html_table(const std::string& output);

    // Check if output contains raw HTML (e.g., from esttab, html)
    bool is_raw_html_output(const std::string& output);

    // Format raw HTML output (no escaping, just wrap in container)
    std::string format_as_raw_html(const std::string& output);

} // namespace xeus_stata

#endif // XEUS_STATA_PARSER_HPP
