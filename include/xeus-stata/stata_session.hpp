#ifndef XEUS_STATA_SESSION_HPP
#define XEUS_STATA_SESSION_HPP

#include <string>
#include <memory>
#include <functional>
#include <vector>

namespace xeus_stata
{
    struct execution_result
    {
        std::string output;
        bool is_error;
        int error_code;
        std::string error_message;
        std::vector<std::string> graph_files;
    };

    class stata_session
    {
    public:
        stata_session(const std::string& stata_path = "");
        ~stata_session();

        // Delete copy constructor and assignment
        stata_session(const stata_session&) = delete;
        stata_session& operator=(const stata_session&) = delete;

        // Execute Stata code and return result
        execution_result execute(const std::string& code);

        // Get Stata version
        std::string get_version();

        // Check if session is ready
        bool is_ready() const;

        // Shutdown the Stata session
        void shutdown();

        // Interrupt current execution
        void interrupt();

        // Get/set macros
        std::string get_macro(const std::string& name);
        void set_macro(const std::string& name, const std::string& value);

    private:
        class impl;
        std::unique_ptr<impl> m_impl;
    };

} // namespace xeus_stata

#endif // XEUS_STATA_SESSION_HPP
