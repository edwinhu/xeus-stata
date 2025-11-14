#ifndef XEUS_STATA_COMPLETION_HPP
#define XEUS_STATA_COMPLETION_HPP

#include <string>
#include <vector>

namespace xeus_stata
{
    class stata_session;

    class completion_engine
    {
    public:
        completion_engine(stata_session* session);

        // Get completions for the given code at cursor position
        std::vector<std::string> get_completions(
            const std::string& code,
            int cursor_pos,
            int& start_pos
        );

    private:
        stata_session* m_session;

        // Get command completions
        std::vector<std::string> get_command_completions(const std::string& prefix);

        // Get variable completions
        std::vector<std::string> get_variable_completions(const std::string& prefix);

        // Get function completions
        std::vector<std::string> get_function_completions(const std::string& prefix);

        // Get macro completions
        std::vector<std::string> get_macro_completions(const std::string& prefix);
    };

} // namespace xeus_stata

#endif // XEUS_STATA_COMPLETION_HPP
