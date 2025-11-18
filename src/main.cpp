#include <iostream>
#include <memory>
#include <signal.h>

#include "xeus/xkernel.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus-zmq/xserver_zmq.hpp"
#include "xeus-zmq/xzmq_context.hpp"

#include "xeus-stata/xinterpreter.hpp"
#include "xeus-stata/xeus_stata_config.hpp"

namespace {
    // Global pointer to interpreter for signal handler access
    // Note: Using raw pointer because signal handlers cannot capture std::unique_ptr
    xeus_stata::interpreter* g_interpreter = nullptr;

    // Signal handler for SIGINT (Ctrl+C)
    void sigint_handler(int signum)
    {
        // Forward interrupt to interpreter
        if (g_interpreter != nullptr)
        {
            g_interpreter->interrupt();
        }
        // Note: Don't restore default handler - we want to keep catching signals
    }
}

int main(int argc, char* argv[])
{
    // Parse command line arguments
    std::string connection_file;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-f" && i + 1 < argc)
        {
            connection_file = argv[i + 1];
            ++i;
        }
        else if (arg == "--version")
        {
            std::cout << "xeus-stata " << XEUS_STATA_VERSION << std::endl;
            return 0;
        }
        else if (arg == "--help" || arg == "-h")
        {
            std::cout << "xeus-stata - A Jupyter kernel for Stata" << std::endl;
            std::cout << std::endl;
            std::cout << "Usage:" << std::endl;
            std::cout << "  xstata -f <connection_file>" << std::endl;
            std::cout << "  xstata --version" << std::endl;
            std::cout << "  xstata --help" << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -f <file>     Specify connection file" << std::endl;
            std::cout << "  --version     Show version information" << std::endl;
            std::cout << "  --help        Show this help message" << std::endl;
            std::cout << std::endl;
            std::cout << "Environment Variables:" << std::endl;
            std::cout << "  STATA_PATH    Path to Stata executable" << std::endl;
            return 0;
        }
    }

    if (connection_file.empty())
    {
        std::cerr << "Error: Connection file not specified" << std::endl;
        std::cerr << "Usage: xstata -f <connection_file>" << std::endl;
        return 1;
    }

    try
    {
        // Load connection configuration
        xeus::xconfiguration config = xeus::load_configuration(connection_file);

        // Create interpreter
        auto interpreter = std::make_unique<xeus_stata::interpreter>();

        // Store raw pointer for signal handler (before moving into kernel)
        g_interpreter = interpreter.get();

        // Install SIGINT handler
        // Use sigaction instead of signal for better portability
        struct sigaction sa;
        sa.sa_handler = sigint_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;  // No special flags needed
        if (sigaction(SIGINT, &sa, nullptr) == -1)
        {
            std::cerr << "Warning: Failed to install SIGINT handler" << std::endl;
            // Continue anyway - not a fatal error
        }

        // Create context
        auto context = xeus::make_zmq_context();

        // Create kernel
        xeus::xkernel kernel(
            config,
            xeus::get_user_name(),
            std::move(context),
            std::move(interpreter),
            xeus::make_xserver_default
        );

        // Start kernel
        std::cout << "Starting xeus-stata kernel..." << std::endl;
        std::cout << "Stata path: " << DEFAULT_STATA_PATH << std::endl;
        std::cout << "(set STATA_PATH environment variable to override)" << std::endl;

        kernel.start();

        // Clean up global reference
        g_interpreter = nullptr;
    }
    catch (const std::exception& e)
    {
        g_interpreter = nullptr;  // Clean up on error
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
