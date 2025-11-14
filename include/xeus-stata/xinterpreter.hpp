#ifndef XEUS_STATA_INTERPRETER_HPP
#define XEUS_STATA_INTERPRETER_HPP

#include <memory>
#include <string>

#include "xeus/xinterpreter.hpp"
#include "nlohmann/json.hpp"

namespace nl = nlohmann;

namespace xeus_stata
{
    class stata_session;
    class completion_engine;
    class inspection_engine;

    class interpreter : public xeus::xinterpreter
    {
    public:
        interpreter();
        virtual ~interpreter();

    private:
        void configure_impl() override;

        nl::json execute_request_impl(
            int execution_counter,
            const std::string& code,
            bool silent,
            bool store_history,
            nl::json user_expressions,
            bool allow_stdin
        ) override;

        nl::json complete_request_impl(
            const std::string& code,
            int cursor_pos
        ) override;

        nl::json inspect_request_impl(
            const std::string& code,
            int cursor_pos,
            int detail_level
        ) override;

        nl::json is_complete_request_impl(
            const std::string& code
        ) override;

        nl::json kernel_info_request_impl() override;

        void shutdown_request_impl() override;

    private:
        std::unique_ptr<stata_session> m_session;
        std::unique_ptr<completion_engine> m_completer;
        std::unique_ptr<inspection_engine> m_inspector;
    };

} // namespace xeus_stata

#endif // XEUS_STATA_INTERPRETER_HPP
