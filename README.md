# Olympus
Unofficial score keeping software for FIRST LEGO League.

## Installation

### Windows

Download and run the latest Windows installer from the [Releases page](https://github.com/barulicm/olympus/releases).

Once the installer finishes, you can run Olympus using the Start Menu shortcut.

### Linux

Download the latest debian package from the [Releases page](https://github.com/barulicm/olympus/releases).

Install the package with the following command, replacing `VERSION` with the version number in your
downloaded file.

```bash
sudo dpkg -i Olympus-VERSION-Linux.deb
```

You can now start the Olympus server with the 'olympus' command.

### Docker

Pull the latest image from the GitHub container registry.

```shell
docker pull ghcr.io/barulicm/olympus:main
```

When running the docker image, make sure to publish exposed ports.

```shell
docker run -P ghcr.io/barulicm/olympus
```

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
