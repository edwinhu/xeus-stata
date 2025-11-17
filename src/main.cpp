#include <iostream>
#include <memory>

#include "xeus/xkernel.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus-zmq/xserver_zmq.hpp"
#include "xeus-zmq/xzmq_context.hpp"

#include "xeus-stata/xinterpreter.hpp"
#include "xeus-stata/xeus_stata_config.hpp"

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
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
