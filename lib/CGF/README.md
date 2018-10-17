# CGF #

This is a simple game framework with a focus on academic applications.
The framework is based on SFML (Simple and Fast Multimedia Library), which already provides most of the low level stuff.

### Quick start ###

  * Clone or download the repository contents
  * The repository contains a precompiled version of the library for Linux 64 bits (compiled on Debian 9), Windows 10 and macOS (Compiled on Sierra).
  * Examples can be found in the "examples" directory. Each has corresponding project files.
  * In order to rebuild the library, the "build" directory contains build files (projects) for Code::Blocks and QtCreator. Each directory also has a README file.

### CMake 
It's possible to use CMake to compile the framework/use it with your own CMake project.
To use it in your own CMakeLists.txt:

  * Clone or download the repository contents
  * Use `add_subdirectory(...)` to "include" the framework
  * Use the target `CGF`

To compile the framework (and examples):

  * Clone or download the repository contents
  * 
```shell
~]$ cd CGF
~]$ mkdir build_cmake
~]$ cd build_cmake
~]$ cmake -DCGF_BUILD_EXAMPLES=TRUE ..
~]$ make
```
* The compiled library and its dependencies will be inside `build_cmake`
* The examples will be inside `bin/examples`

Besides building (or not) the examples there are two other options available through CMake:

  * `-DCGF_DISABLE_LOGGING`(FALSE) - Disable or not the debug messages printed by the framework
  * `-DBUILD_SHARED_LIBS`(TRUE) -  If false will build the framework as a static library


### Credits ###

* Tiled map loader (TMX) by Matt Marchant <https://github.com/fallahn/sfml-tmxloader>

### Contact ###

* Dr. Marcelo Cohen (marcelo.cohen@pucrs.br)
