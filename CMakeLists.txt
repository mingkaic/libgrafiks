cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME_STR libgrafiks)
project(${PROJECT_NAME_STR} C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE MAIN_SRC provided/*.cpp)
file(GLOB_RECURSE LIB_SRC lib/src/*.cpp)

include_directories(${Qt5Widgets_INCLUDE_DIRS} provided lib/include test)

set(MAINEXEC MainPage)
add_executable(${MAINEXEC} ${MAIN_SRC} ${LIB_SRC})
target_link_libraries(${MAINEXEC} Qt5::Widgets Qt5::Core Qt5::Gui)