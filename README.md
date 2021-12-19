# URCL Parser
A parser for the URCL language written for C, C++, and Python.

## Building the Library
To build this library you with need to run ./src/build.py from the project root directory using Python 3.

By default the script will build all versions of the library, invoke GCC to build the library binaries, and generate the documentation. To build only a specific version or just the documentation, use any number of the following arguments when invoking the script:
| Task Argument | Description                                                       |
| ------------- | ----------------------------------------------------------------- |
| C             | Output the C header and C docs only.                              |
| C++           | Output the C++ header and combined C++/C docs only.               |
| Python        | Output the Python module, library binaries, and Python docs only. |
| Docs          | Output the public documentation only.                             |

To use a compiler other than GCC for building the library binaries, create ./src/compile.py and write your own script. It will automatically be invoked by the build script instead of GCC.

## Using the Library

### C
To use this library in C, include ./release/c/urcl.h in your source. All structures and functions are prefixed with `URCL_`.

### C++
To use this library in C++, include ./release/cpp/urcl.hpp in your source. All classes and functions can be found in the `URCL` namespace. Compile with C++11 or higher.

### Python
The Python bindings are still in progress.