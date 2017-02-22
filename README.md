# LibGrafiks

### Directory Structure

`lib` directory contains the core library

`provided` directory contains qt interface and test client

`test` directory contains custom test object per panel which is used by the test client

### How to Test

1. It is recommended to make a build directory

> mkdir build && cd build

2. Run CMakeLists.txt

> cmake -DCMAKE_PREFIX_PATH=<path/to/qt/dir>/clang_64/lib/cmake <path/to/graphicsLibrary> && make

3. Run binary MainPage

> ./MainPage

There is the option to specify the test directory by

> ./MainPage -testdir=/relative/path/to/tests

By default the test path is the executable's directory

For assignment 2 pages 4, and 5 copy contents of /test/simp/ to the desired test directory or executable directory