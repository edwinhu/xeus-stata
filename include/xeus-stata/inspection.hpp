#ifndef XEUS_STATA_INSPECTION_HPP
#define XEUS_STATA_INSPECTION_HPP

#include <string>

namespace xeus_stata
{
    class stata_session;

    class inspection_engine
    {
    public:
        inspection_engine(stata_session* session);

        // Get inspection info for code at cursor position
        std::string get_inspection(
            const std::string& code,
            int cursor_pos,
            int detail_level
        );

    private:
        stata_session* m_session;

        // Get help for a Stata command
        std::string get_command_help(const std::string& command);

        // Get info about a variable
        std::string get_variable_info(const std::string& variable);
    };

} // namespace xeus_stata

#endif // XEUS_STATA_INSPECTION_HPP
