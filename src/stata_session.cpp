#include "xeus-stata/stata_session.hpp"
#include "xeus-stata/stata_parser.hpp"
#include "xeus-stata/xeus_stata_config.hpp"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#if defined(XEUS_STATA_PLATFORM_LINUX) || defined(XEUS_STATA_PLATFORM_MACOS)
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <signal.h>
    #include <poll.h>
    #if defined(__APPLE__)
        #include <util.h>
    #else
        #include <pty.h>
    #endif
#endif

namespace xeus_stata
{
    class stata_session::impl
    {
    public:
        impl(const std::string& stata_path)
            : m_stata_path(stata_path)
            , m_master_fd(-1)
            , m_pid(-1)
            , m_ready(false)
        {
            if (m_stata_path.empty())
            {
                // Try environment variable first
                const char* env_path = std::getenv("STATA_PATH");
                if (env_path && env_path[0] != '\0')
                {
                    m_stata_path = env_path;
                }
                else
                {
                    m_stata_path = DEFAULT_STATA_PATH;
                }
            }

            start_stata();
        }

        ~impl()
        {
            shutdown();
        }

        void start_stata()
        {
#if defined(XEUS_STATA_PLATFORM_LINUX) || defined(XEUS_STATA_PLATFORM_MACOS)
            char slave_name[256];
            int slave_fd;

            // Open pseudo-terminal
            if (openpty(&m_master_fd, &slave_fd, slave_name, nullptr, nullptr) == -1)
            {
                throw std::runtime_error("Failed to open pseudo-terminal: " +
                                       std::string(strerror(errno)));
            }

            // Fork process
            m_pid = fork();
            if (m_pid == -1)
            {
                close(m_master_fd);
                close(slave_fd);
                throw std::runtime_error("Failed to fork process: " +
                                       std::string(strerror(errno)));
            }

            if (m_pid == 0)
            {
                // Child process
                close(m_master_fd);

                // Redirect stdin, stdout, stderr to slave PTY
                dup2(slave_fd, STDIN_FILENO);
                dup2(slave_fd, STDOUT_FILENO);
                dup2(slave_fd, STDERR_FILENO);
                close(slave_fd);

                // Execute Stata
                // Use -q for quiet startup (no banner)
                execlp(m_stata_path.c_str(), m_stata_path.c_str(), "-q", nullptr);

                // If exec fails
                std::cerr << "Failed to execute Stata: " << strerror(errno) << std::endl;
                _exit(1);
            }

            // Parent process
            close(slave_fd);

            // Set non-blocking mode
            int flags = fcntl(m_master_fd, F_GETFL, 0);
            fcntl(m_master_fd, F_SETFL, flags | O_NONBLOCK);

            // Wait for Stata to start and show prompt
            std::string startup_output = read_until_prompt(5000); // 5 second timeout

            // Set up initial configuration
            // Disable pagination
            write_command("set more off");
            // Set line size for better output
            write_command("set linesize 200");

            m_ready = true;
#else
            throw std::runtime_error("Windows support not yet implemented");
#endif
        }

        execution_result execute(const std::string& code)
        {
            if (!m_ready)
            {
                throw std::runtime_error("Stata session not ready");
            }

            // Generate unique marker for detecting command completion
            std::string marker = generate_execution_marker();

            // Wrap code with marker
            std::string wrapped_code = code + "\ndisplay \"__MARKER__" + marker + "__\"";

            // Write command
            write_command(wrapped_code);

            // Read output until we see the marker
            std::string output = read_until_marker("__MARKER__" + marker + "__", 30000); // 30 second timeout

            // Parse the output
            return parse_execution_output(output);
        }

        std::string get_version()
        {
            auto result = execute("display c(version)");
            if (!result.is_error && !result.output.empty())
            {
                // Trim whitespace
                std::string version = result.output;
                version.erase(0, version.find_first_not_of(" \t\n\r"));
                version.erase(version.find_last_not_of(" \t\n\r") + 1);
                return version;
            }
            return "Unknown";
        }

        bool is_ready() const
        {
            return m_ready;
        }

        void shutdown()
        {
            if (m_pid > 0)
            {
#if defined(XEUS_STATA_PLATFORM_LINUX) || defined(XEUS_STATA_PLATFORM_MACOS)
                // Send exit command
                write_command("exit, clear");

                // Wait for process to terminate (with timeout)
                int status;
                int wait_result = waitpid(m_pid, &status, WNOHANG);

                if (wait_result == 0)
                {
                    // Process still running, force kill
                    kill(m_pid, SIGTERM);
                    usleep(100000); // Wait 100ms
                    wait_result = waitpid(m_pid, &status, WNOHANG);

                    if (wait_result == 0)
                    {
                        // Still running, force kill
                        kill(m_pid, SIGKILL);
                        waitpid(m_pid, &status, 0);
                    }
                }

                m_pid = -1;
#endif
            }

            if (m_master_fd >= 0)
            {
                close(m_master_fd);
                m_master_fd = -1;
            }

            m_ready = false;
        }

        void interrupt()
        {
#if defined(XEUS_STATA_PLATFORM_LINUX) || defined(XEUS_STATA_PLATFORM_MACOS)
            if (m_pid > 0)
            {
                kill(m_pid, SIGINT);
            }
#endif
        }

        std::string get_macro(const std::string& name)
        {
            auto result = execute("display `" + name + "'");
            if (!result.is_error)
            {
                return result.output;
            }
            return "";
        }

    private:
        void write_command(const std::string& command)
        {
#if defined(XEUS_STATA_PLATFORM_LINUX) || defined(XEUS_STATA_PLATFORM_MACOS)
            std::string cmd = command + "\n";
            ssize_t written = write(m_master_fd, cmd.c_str(), cmd.length());
            if (written != static_cast<ssize_t>(cmd.length()))
            {
                throw std::runtime_error("Failed to write to Stata process");
            }
#endif
        }

        std::string read_until_prompt(int timeout_ms)
        {
            return read_until_marker(".", timeout_ms);
        }

        std::string read_until_marker(const std::string& marker, int timeout_ms)
        {
#if defined(XEUS_STATA_PLATFORM_LINUX) || defined(XEUS_STATA_PLATFORM_MACOS)
            std::string output;
            char buffer[4096];

            struct pollfd pfd;
            pfd.fd = m_master_fd;
            pfd.events = POLLIN;

            int elapsed = 0;
            const int poll_interval = 100; // 100ms

            while (elapsed < timeout_ms)
            {
                int ret = poll(&pfd, 1, poll_interval);

                if (ret > 0 && (pfd.revents & POLLIN))
                {
                    ssize_t n = read(m_master_fd, buffer, sizeof(buffer) - 1);
                    if (n > 0)
                    {
                        buffer[n] = '\0';
                        output += buffer;

                        // Check if we've received the marker
                        if (output.find(marker) != std::string::npos)
                        {
                            // Remove the marker and everything after it
                            size_t pos = output.find(marker);
                            output = output.substr(0, pos);
                            break;
                        }
                    }
                }

                elapsed += poll_interval;
            }

            return output;
#else
            return "";
#endif
        }

        std::string m_stata_path;
        int m_master_fd;
        pid_t m_pid;
        bool m_ready;
    };

    // stata_session public interface implementation
    stata_session::stata_session(const std::string& stata_path)
        : m_impl(std::make_unique<impl>(stata_path))
    {
    }

    stata_session::~stata_session() = default;

    execution_result stata_session::execute(const std::string& code)
    {
        return m_impl->execute(code);
    }

    std::string stata_session::get_version()
    {
        return m_impl->get_version();
    }

    bool stata_session::is_ready() const
    {
        return m_impl->is_ready();
    }

    void stata_session::shutdown()
    {
        m_impl->shutdown();
    }

    void stata_session::interrupt()
    {
        m_impl->interrupt();
    }

    std::string stata_session::get_macro(const std::string& name)
    {
        return m_impl->get_macro(name);
    }

    void stata_session::set_macro(const std::string& name, const std::string& value)
    {
        // Execute local/global macro assignment
        m_impl->execute("local " + name + " \"" + value + "\"");
    }

} // namespace xeus_stata
