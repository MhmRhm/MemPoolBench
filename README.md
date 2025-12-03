# SeeMake

A feature-packed, ready-to-use, cross-platform CMake template with testing,
static and dynamic checks, coverage reports, and more.

This template comes with a [tutorial](https://mhmrhm.github.io/tutorials/posts/see-make/)
and some working examples, so be sure to read on.

1. [Included Features](#included-eatures)
2. [Notes Before You Begin](#notes-before-you-begin)
3. [Setting Up Linux](#setting-up-linux)
4. [Setting Up Windows](#setting-up-windows)
5. [Setting Up Mac](#setting-up-mac)
6. [Final Step](#final-step)
7. [First Step](#first-step)

## Included Features:

- **Well-Structured and Easy to Extend**: The template is organized for easy
customization and expansion.
- **One-Command Workflow**: Configure, build, test, and package your project with
a single command.
- **Minimal Demo Code**: Provides examples for building an object library, a
static library, a shared library, and a bootstrapped executable that links
against them.
- **Out-of-the-Box Support**: Comes with GoogleTest, Google Benchmark,
Boost.Test, Boost.Asio, and Protobuf, each with accompanying demo code.
- **Simple Dependency Management**: Most dependencies are fetched using CMake's
FetchContent or vcpkg, so there's no need to manually build and install Boost or
Google libraries.
- **Static and Dynamic Checks**: Uses Memcheck and Cppcheck to perform dynamic
and static checks.
- **Git Information**: Easily include Git details like commit hash and branch
name in your binary.
- **Automated Coverage Reports**: Generates test coverage reports automatically.
- **Automated Documentation**: Generates documentation with UML diagrams
automatically.
- **Code Formatting**: Automatically formats and styles your code before each
build.
- **Easy Installation**: Install targets with a single command, with well-defined
install targets and demo code.
- **Simple Packaging**: Create installers and packages for your libraries and
executables with one command.
- **CMake Config Files**: Generates CMake config files so others can easily link
against your project.
- **Cross-Platform Support**: All that on Windows, Linux and macOS!
(Dynamic checks for macOS is still a work in progress)

This work is based on material from
[Modern CMake for C++](https://github.com/PacktPublishing/Modern-CMake-for-Cpp)
by Rafał Świdziński, which is licensed under the MIT license. It is one of the
most useful books I have read.

For those using this template who want a deeper understanding, I've provided a
tutorial on this template at [DotBashHistory](https://mhmrhm.github.io/tutorials/posts/see-make/).
I highly recommend that you review the tutorial or at least examine each file in
this template to understand them. You will likely need to modify these files at
some point.

One of the examples built with this template is a terminal-based Tower of Hanoi
game that follows the Model–View–Controller (MVC) design pattern. You can see it
in action [here](https://github.com/MhmRhm/FTowerX). Another example, showcasing
a collection of practical recipes for Boost.Asio and Google Protobuf, is
available [here](https://github.com/MhmRhm/asio-recipes).

## Notes Before You Begin

I assume you will read this file in full before building the template. Below, I
will outline the necessary steps to prepare your system for the build.

Before continuing, note that this template uses CMake's FetchContent and vcpkg to
manage dependencies. For large repositories such as Boost, the setup process may
take some time, but you'll get library versions that are often many releases
ahead of what system package managers provide. If the vcpkg build cache becomes
too large, you can free up space by removing the `buildtrees` and `downloads`
directories in your vcpkg installation.

Generating test coverage reports requires debug symbols, so coverage targets
won't build for Release configurations.

Keep in mind that on Windows, file paths cannot exceed 260 characters. To prevent
issues, use short folder names and **avoid spaces in directory or file names**.

All necessary path variables used by this template are configured in the
`CMakePresets.json` file. If your local installation paths for dependencies like
vcpkg or Qt differ from the defaults, modify the entries in this file
accordingly. For instance, when developing on Windows and using the Clang Release
preset, locate the preset in the `.json` file and update its `cacheVariables`
section to reflect your environment.

If you're developing Qt applications in an environment other than Qt Creator,
running an executable that depends on Qt libraries may fail. This typically
happens because the executable cannot locate the required Qt libraries at
runtime.

There are several ways to resolve this issue:

1. **Configure environment variables in VS Code:**
   Add a [`launch.json`](https://code.visualstudio.com/docs/debugtest/debugging-configuration)
   file and define the necessary environment variables so that VS Code sets them
   automatically when launching your application.

2. **Set environment variables manually:**
   Add the required Qt library paths to your environment variables in the
   terminal, then run your executable from that terminal session.
   If you’re using VS Code and have the CMake Tools extension installed, you can
   configure these variables directly in the editor. Open the VS Code settings,
   search for `cmake.environment`, and add the necessary paths there.

3. **Use the Qt deployment tool (Windows):**
   Run the *windeployqt* tool located at for example `C:\Qt\6.9.3\msvc2022_arm64\bin\windeployqt6.exe`
   and provide it with the path to your executable. This will copy the required
   Qt libraries next to your binary.

If you’re using Qt Creator, the IDE automatically configures the necessary
environment variables, so no manual setup is needed.

To manually set the environment variables:

**On Linux:**

```bash
export LD_LIBRARY_PATH="/home/<username>/Qt/6.9.3/gcc_arm64/lib:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="/home/<username>/Qt/6.9.3/gcc_arm64/bin:$LD_LIBRARY_PATH"
```

**On Windows (PowerShell):**

```powershell
$Env:Path += ";C:\Qt\6.9.3\msvc2022_arm64\lib;C:\Qt\6.9.3\msvc2022_arm64\bin"
```

One of the steps in the workflows defined in the `CMakePresets.json` file
involves running tests. If your application and, consequently, your tests depend
on Qt, you must add the Qt libraries to the path before running the workflows.

One last thing, I've encountered a situation on Windows where having Strawberry
Perl installed can interfere with or disable Cppcheck. Just something to keep in
mind.

## Setting Up Linux

At a minimum, you'll need to install Git and CMake to get started. You can
install CMake either from your system's default repositories or by adding the
Kitware repository, which provides the latest versions.

To install CMake and Git from the repositories, run:

```bash
sudo apt-get -y install git cmake
```

Or for the latest version of CMake, visit the
[Kitware APT Repository](https://apt.kitware.com).

Next, install the compiler, debugger and some basic packages by running:

```bash
sudo apt-get install -y build-essential libgl1-mesa-dev gdb
sudo apt-get install -y pkg-config autoconf autoconf-archive
sudo apt-get install -y libtool curl zip unzip tar
sudo apt-get install -y linux-headers-$(uname -r)
sudo apt-get install -y bison flex
```

This template depends on vcpkg. Follow its
[official documentation](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started)
to install it. The `CMakePresets.json` file assumes that vcpkg is located in the
user's `\home\<username>\vcpkg` directory. This template installs its
dependencies using a vcpkg
[manifest file](https://learn.microsoft.com/en-us/vcpkg/consume/manifest-mode).

Installing vcpkg is straightforward and can be done with the following commands:

```bash
cd $HOME
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg && ./bootstrap-vcpkg.sh
```

At this point, you may proceed with a trial-and-error approach. This template
includes a `CMakePresets.json` file with predefined workflows. To get started,
run the following command and review the output. If any packages are missing,
install them and try again.

```bash
cmake --workflow --list-presets
cmake --workflow --fresh --preset linux-default-release
```

Alternatively, you can run the following commands to install the remaining
required packages all at once:

```bash
sudo apt-get -y install doxygen graphviz mscgen dia
sudo apt-get -y install clang-format
sudo apt-get -y install valgrind gawk
sudo apt-get -y install cppcheck
sudo apt-get -y install lcov
```

Optionally, to install Qt, download the [online installer](https://www.qt.io/download-open-source)
and follow the setup instructions. After installation, update the `QTDIR` path in
the `CMakePresets.json` file accordingly. If your environment supports GUI
applications, you can complete the installation using the standard graphical
setup. Otherwise, you can run the online installer through its command-line
interface. To install all required dependencies and Qt, run the following
commands:

```bash
# Install Qt for X11 requirements. For more details, see the following links:
# https://doc.qt.io/qt-6/linux-requirements.html
# https://doc.qt.io/Qt-6/get-and-install-qt-cli.html

sudo apt install \
   libfontconfig1-dev libfreetype-dev libgtk-3-dev libx11-dev libx11-xcb-dev \
   libxcb-cursor-dev libxcb-glx0-dev libxcb-icccm4-dev libxcb-image0-dev \
   libxcb-keysyms1-dev libxcb-randr0-dev libxcb-render-util0-dev \
   libxcb-shape0-dev libxcb-shm0-dev libxcb-sync-dev libxcb-util-dev \
   libxcb-xfixes0-dev libxcb-xkb-dev libxcb1-dev libxext-dev libxfixes-dev \
   libxi-dev libxkbcommon-dev libxkbcommon-x11-dev libxrender-dev

# Download and install Qt base components:
wget https://d13lb3tujbc8s0.cloudfront.net/onlineinstallers/qt-online-installer-linux-arm64-4.10.0.run
chmod +x ./qt-online-installer-linux-arm64-4.10.0.run
./qt-online-installer-linux-arm64-4.10.0.run install

# After the base installation, install additional components using the MaintenanceTool:
cd ~/Qt/
./MaintenanceTool install qt.qt6.6100.addons

# Install Qt IDE if needed.
./MaintenanceTool install qt.tools.qtcreator_gui
```

The following tools are used in this project:

- **Doxygen** and **Graphviz**: Generate project documentation with UML graphs.
- **Clang-Format**: Automatically applies standard formatting to the files.
- **Valgrind**: Checks for memory leaks during tests.
- **Cppcheck**: Performs static analysis to find potential bugs.
- **Lcov**: Generates coverage reports for tests in Debug.

Many of these reports are available in HTML format and can be easily served. To
serve a report, navigate to the corresponding build directory and run:

```bash
python3 -m http.server <port-number>
```

Then, open the provided address in your browser to view the report.

Examples:

1. **Documentation**:
   ```bash
   cmake --preset linux-default-debug
   cmake --build --preset linux-default-debug --target doxygen-libsee_static
   cd ../SeeMake-build-linux-default-debug/doxygen-libsee_static/
   python3 -m http.server 8172
   # Go to localhost:8172 in your browser to view the documentation
   ```

<p align="center"><img src="https://i.postimg.cc/yNDzJxdZ/temp-Imageftu3t-A.avif" alt="Documentation"></img></p>

2. **Memory Check Report**:
   ```bash
   ulimit -n 65536
   cmake --preset linux-default-debug
   cmake --build --preset linux-default-debug --target memcheck-google_test_libsee
   cd ../SeeMake-build-linux-default-debug/valgrind-google_test_libsee/
   python3 -m http.server 8172
   # Go to localhost:8172 to view the test results
   ```

<p align="center"><img src="https://i.postimg.cc/JzFsRRbm/temp-Image-V6MDoy.avif" alt="Valgrind"></img></p>

3. **Coverage Report**:
   ```bash
   cmake --preset linux-default-debug
   cmake --build --preset linux-default-debug --target coverage-google_test_libsee
   cd ../SeeMake-build-linux-default-debug/coverage-google_test_libsee/
   python3 -m http.server 8172
   # Go to localhost:8172 to view the test coverage report
   ```

<p align="center"><img src="https://i.postimg.cc/ydMhxF9L/temp-Image-HDo-D1i.avif" alt="Coverage"></img></p>

## Setting Up Windows

Setting up on Windows is quite different from Linux. On Windows, you'll need to
manually find and install the required packages, making sure to add them to your
system's Path. Most software you install will have an option to add it to the
Path during installation, so no worries there. For any software that doesn't
offer that option, it's handled in the `CMakePresets.json` file.

The main task on Windows is to carefully download and install the required
software to set up this template. If you're developing on Windows on ARM, be sure
to download the correct binary from the software provider. Some offer custom
builds specifically for ARM architecture.

Here's the list of software you need to download and install:

1. **[Git](https://git-scm.com/download/win)**: The default settings should be
fine. I usually change the default branch name to "main" and disable Git GUI.
   
2. **[CMake](https://cmake.org/download/)**: Ensure that the installer adds CMake
to the Path.

3. **[Visual Studio 2022](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022)**:
Select the "Desktop development with C++" option.

4. **[Qt](https://www.qt.io/download-open-source)**: [Optional] Follow standard
setup. During installation, under Additional Libraries for your chosen Qt version,
select the libraries you need. You'll likely not require any debug information
files. In the Build Tools section, uncheck CMake. If you already have Qt
installed, run the Maintenance Tool in your installation directory to add or
install another version.

5. **[vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell#1---se)**:
Follow step 1 of standard vcpkg setup. The `CMakePresets.json` assumes it is
installed directly under the C directory (`C:\vcpkg`).

This template includes a `CMakePresets.json` file with predefined workflows. To
start, run the following command and review the output. If any packages are
missing, you can install them and repeat the process:

```bash
cmake --workflow --list-presets
cmake --workflow --fresh --preset windows-default-release
```

Alternatively, you can follow the rest to install all required packages:

6. **[Doxygen](https://www.doxygen.nl/download.html)**: Standard installation.

7. **[Graphviz](https://graphviz.org/download/)**: Make sure you choose the
option to add this to the Path.

8. **[LLVM](https://github.com/llvm/llvm-project/releases/)**: There are many
items. For my Windows VM on Apple Silicon, I chose `LLVM-18.1.8-woa64.exe`.
This package installs `llvm-cov`, `clang-format` and Clang compilers. Make sure
you select the option to add it to the Path. On Windows coverage reports are
available only with `windows-clang-debug` preset.

To generate coverage reports on Windows:

```bash
cmake --preset windows-clang-debug
cmake --build --preset windows-clang-debug --target coverage-google_test_libsee
cd ../SeeMake-build-windows-clang-debug/coverage-google_test_libsee/
python3 -m http.server 8172
# Go to localhost:8172 to view the test coverage report
```

<p align="center"><img src="https://i.postimg.cc/L5vb1CT2/temp-Imagem-YQb-Xr.avif" alt="Coverage"></img></p>

9. **[Cppcheck](https://cppcheck.sourceforge.io/)**: Standard installation.

10. **[NSIS](https://nsis.sourceforge.io/Download)**: Standard installation.

11. **[Ninja](https://ninja-build.org/)**: If you already have Qt installed, you
probably have the executable at `C:/Qt/Tools/Ninja`. In that case, change the
`CMAKE_MAKE_PROGRAM` in `CMakePresets.json` from `C:/Program Files/ninja/ninja`
to the one provided by Qt. If Qt is not installed, or it was installed without
Ninja, copy the downloaded executable to `C:/Program Files/ninja/`.

## Setting Up Mac

Mac support at the moment lacks dynamic checks. To set up Mac for this template,
follow these steps (replace the `<user>` with correct value):

1. **Install Homebrew:**

   Run the following command to install Homebrew, the macOS package manager.
   After the installation completes, Homebrew will display a few commands that
   you need to run manually to add it to your shell environment.

   ```zsh
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

2. **Install required dependencies:**

   Use Homebrew to install the necessary tools:

   ```zsh
   brew install git cmake llvm
   brew install cppcheck clang-format doxygen graphviz
   brew install autoconf autoconf-archive automake libtool pkg-config
   ```

3. **Install vcpkg:**

   This template depends on vcpkg. Follow its
   [official documentation](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started)
   to install it. The `CMakePresets.json` file assumes that vcpkg is located in
   the user's `/Users/<username>/vcpkg` directory. This template installs its
   dependencies using a vcpkg [manifest file](https://learn.microsoft.com/en-us/vcpkg/consume/manifest-mode).

   Installing vcpkg is straightforward and can be done with the following
   commands:

   ```zsh
   cd $HOME
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg && ./bootstrap-vcpkg.sh
   ```

4. **Install Qt:**

   Optionally, to install Qt, download the [online installer](https://www.qt.io/download-open-source)
   and follow the setup instructions. After installation, update the `QTDIR` path
   in the `CMakePresets.json` file accordingly.

## Final Step

Use the following commands to see the available presets and build with the one
that matches your setup:

```bash
cmake --workflow --list-presets
cmake --workflow --fresh --preset linux-default-release
```

This repository contains two branches. In addition to the `main` branch, there
is a helper branch designed to make it easier to adapt this template for your
own project. All files, folders, and CMake targets in the template include the
keyword `see` in their names. The helper branch automates the process of
renaming these items—files, directories, and internal references—to match your
chosen project name.

Run `git show -s origin/rename-project` and see the commit message which
explains how to use this feature:

```
Generalize project by renaming all references from `See` to a new project name

This branch is used to generate a patch that can be applied to the main branch.
Before applying the patch, every occurrence of `ProjectName` is replaced with
a chosen project name. This approach automatically updates all file names,
folder names, and build targets with minimal effort.

Example workflow in a Bash terminal:

  git checkout rename-project
  git format-patch main --stdout > projectname.patch
  sed -i "s/ProjectName/<new-project-name>/g" projectname.patch
  git checkout main
  git apply projectname.patch
  rm projectname.patch

To restart the process from scratch:

  git restore .
  git clean -xdff

Note: This template defines two major targets — a library and an executable
that links to it. For example, if you choose `computation` as the new project
name, the resulting targets will be `libcomputation` and `computation_app`.
```

## First Step

One man's final step is another man's first step. After developing your
library using this template, your users will need to add it as an external
dependency. One easy way to do this is by using CMake's `FetchContent`. This
template has already set things up, so your users can add your library like
this:

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.30.0)
project(Extension VERSION 0.0.0 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(Libsee
    GIT_TAG main
    GIT_REPOSITORY https://github.com/MhmRhm/SeeMake.git
)
FetchContent_MakeAvailable(Libsee)

# or after installation:
# find_package(libsee)

add_executable(extension main.cpp)
target_link_libraries(extension PRIVATE see::libsee_shared see::precompiled)
```

In their `main.cpp`, they might write:

```cpp
// main.cpp

#include <iostream>
#include <format>
#include "libsee/see.h"

int main() {
  std::cout << getVersion() << std::endl;
}
```

Finally, they can compile the project easily using the `CMakePresets.json` file
or setup their own presets:

```bash
cmake --workflow --fresh --preset linux-default-release
```

This template works well with the recommended extensions for C++ development in
VSCode, so be sure to check them out.

This template is tested on Linux, Windows and to some extend on Mac OS. I hope
you find this useful. Please feel free to reach out if you have any improvements
or suggestions.
