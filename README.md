# xeus-stata

A Jupyter kernel for Stata based on the xeus framework.

## Overview

xeus-stata is a native C++ implementation of a Jupyter kernel for the Stata statistical software, built on the [xeus](https://github.com/jupyter-xeus/xeus) framework. Unlike Python-based kernels, xeus-stata provides native performance and tight integration with the Jupyter ecosystem.

## Features

- **Code Execution**: Run Stata commands interactively in Jupyter notebooks
- **Code Completion**: Intelligent completion for commands, variables, and functions
- **Code Inspection**: Access Stata help and documentation inline
- **Graph Display**: Automatic detection and display of Stata graphs
- **Error Handling**: Clear error messages with Stata error codes
- **Cross-Platform**: Supports Linux, macOS, and Windows

## Installation

### Prerequisites

- A licensed installation of Stata (version 14 or later recommended)
- CMake 3.8 or later
- C++17 compatible compiler
- Jupyter or JupyterLab

### Via Conda (Coming Soon)

```bash
mamba install -c conda-forge xeus-stata
```

### From Source

```bash
# Clone the repository
git clone https://github.com/jupyter-xeus/xeus-stata.git
cd xeus-stata

# Create a build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX

# Build
make -j$(nproc)

# Install
make install

# Register the kernel
jupyter kernelspec install ../share/jupyter/kernels/xeus-stata --sys-prefix
```

## Usage

After installation, you can use xeus-stata in Jupyter:

```bash
# Launch Jupyter Lab
jupyter lab

# Or Jupyter Notebook
jupyter notebook
```

Then create a new notebook and select "Stata" as the kernel.

### Example

```stata
* Load example dataset
sysuse auto, clear

* Summary statistics
summarize price mpg weight

* Create a scatter plot
scatter mpg weight

* Run regression
regress price mpg weight foreign
```

## Configuration

### Stata Path

xeus-stata will automatically detect Stata if it's installed in a standard location. If Stata is installed in a non-standard location, set the `STATA_PATH` environment variable:

```bash
export STATA_PATH="/path/to/stata"
```

## Development Status

xeus-stata is currently in **early development**. Current status:

- [x] Basic project structure
- [x] Console mode Stata interface (Linux/macOS)
- [ ] Windows console mode support
- [ ] Complete code completion
- [ ] Complete code inspection
- [ ] Graph display integration
- [ ] Comprehensive testing
- [ ] Documentation

## Architecture

xeus-stata consists of several key components:

1. **xinterpreter**: Implements the Jupyter kernel protocol via xeus
2. **stata_session**: Manages the Stata process and communication
3. **stata_parser**: Parses Stata output for results, errors, and graphs
4. **completion**: Provides code completion functionality
5. **inspection**: Provides code inspection and help

## Comparison with stata_kernel

| Feature | xeus-stata | stata_kernel |
|---------|------------|--------------|
| Language | C++ | Python |
| Performance | Native | Python overhead |
| Dependencies | C++ libraries | Python + packages |
| License | BSD-3-Clause | GPL-3.0 |
| Widget Support | xwidgets (planned) | Limited |

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Setup

```bash
# Create development environment
mamba create -n xeus-stata-dev -c conda-forge \
    xeus xeus-zmq xtl nlohmann_json cppzmq \
    cmake gtest ninja jupyterlab cxx-compiler

conda activate xeus-stata-dev

# Build in debug mode
mkdir build-debug && cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

## License

xeus-stata is licensed under the BSD-3-Clause license. See [LICENSE](LICENSE) for details.

## Acknowledgments

- [xeus](https://github.com/jupyter-xeus/xeus) - The underlying kernel framework
- [stata_kernel](https://github.com/kylebarron/stata_kernel) - Inspiration and reference implementation
- The Jupyter project

## Links

- **Documentation**: https://xeus-stata.readthedocs.io (coming soon)
- **Source Code**: https://github.com/jupyter-xeus/xeus-stata
- **Issue Tracker**: https://github.com/jupyter-xeus/xeus-stata/issues
- **Stata**: https://www.stata.com
