# LibGrafiks

### Directory Structure

`lib` directory contains the core library

`provided` directory contains qt interface and test client

`test` directory contains custom test object per panel which is used by the test client

### How to Test

1. It is recommended to make a build directory

> mkdir build && cd build

2. Run CMakeLists.txt

> cmake -DCMAKE_PREFIX_PATH=<path/to/qt/dir> <path/to/graphicsLibrary> && make

3. Run binary MainPage

> ./MainPage