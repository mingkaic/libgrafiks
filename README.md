# LibGrafiks

### Directory Structure

`lib` directory contains the core library

`provided` directory contains qt interface and test client

`test` directory contains custom test object per panel which is used by the test client

### How to Test

1. It is recommended to make a build directory

> mkdir build && cd build

2. Run CMakeLists.txt

> cmake -DCMAKE_PREFIX_PATH=<path/to/qt/cmakedir> <path/to/graphicsLibrary>

Path to qt directory can take the form of: 

For windows and linux installations:

- <path\to\qtdirectory>\5.8\msvc2015_64\lib\cmake

For mac installations:

- <path\to\qtdirectory>/clang_64/lib/cmake

Run generated make file



3. Run binary MainPage

> ./MainPage

There is the option to specify the test directory by

> ./MainPage -testdir=/relative/path/to/tests

By default the test path is the executable's directory

For assignment 2 pages 4, and 5 copy contents of /test/simp/ to the desired test directory or working directory