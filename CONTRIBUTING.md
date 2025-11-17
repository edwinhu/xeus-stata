# Contributing to xeus-stata

Thank you for your interest in contributing to xeus-stata!

## Development Setup

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.8 or later
- A licensed installation of Stata (version 14+)
- Jupyter or JupyterLab

### Setting up the development environment

```bash
# Create conda environment with dependencies
mamba create -n xeus-stata-dev -c conda-forge \
    xeus xeus-zmq xtl nlohmann_json cppzmq \
    cmake gtest ninja jupyterlab cxx-compiler

conda activate xeus-stata-dev
```

### Building from source

```bash
# Clone the repository
git clone https://github.com/jupyter-xeus/xeus-stata.git
cd xeus-stata

# Create build directory
mkdir build && cd build

# Configure (Debug mode for development)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX

# Build
make -j$(nproc)

# Install
make install
```

### Running tests

```bash
cd build
ctest --output-on-failure
```

## Code Style

- Follow C++17 best practices
- Use 4 spaces for indentation
- Keep line length under 100 characters where reasonable
- Use meaningful variable and function names
- Add comments for complex logic

## Submitting Changes

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Add tests for new functionality
5. Ensure all tests pass
6. Commit your changes (`git commit -m 'Add amazing feature'`)
7. Push to your fork (`git push origin feature/amazing-feature`)
8. Open a Pull Request

## Reporting Issues

When reporting issues, please include:

- xeus-stata version
- Stata version
- Operating system
- Steps to reproduce the issue
- Expected vs. actual behavior
- Any error messages or logs

## Areas for Contribution

- Windows console mode support
- Stata Automation API support (Windows/macOS)
- Enhanced code completion
- Improved graph handling
- Documentation improvements
- Test coverage
- Performance optimizations

## Questions?

Feel free to open an issue for discussion!
