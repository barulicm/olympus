# Olympus
Unofficial score keeping software for FIRST LEGO League.

## User Documentation

User documentation is available in [this repository's wiki](https://github.com/barulicm/olympus/wiki).

## Developer Quick Start

Olympus is largely written in C++, uses CMake as its build tool, and uses vcpkg for dependency management.

### Dependencies

You will need these tools installed to contribute to Olympus.

* [git](https://git-scm.com/)
* [CMake](https://cmake.org/)
* A C++ compiler
  * Windows: [Visual Studio](https://visualstudio.microsoft.com/)
  * Linux: [gcc](https://gcc.gnu.org/)

### Clone and Build

Download the project code to your computer:

```shell
git clone --recurse-submodules https://github.com/barulicm/olympus.git
```

Use CMake workflow presets to build and test for your machine:

```shell
# For Linux:
cmake --workflow --preset linux-debug-buildandtest
# For Windows:
cmake --workflow --preset windows-debug-buildandtest
```
