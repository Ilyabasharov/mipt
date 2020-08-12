cmake_minimum_required(VERSION 3.1)

project(SFMLTest)

## If you want to link SFML statically
# set(SFML_STATIC_LIBRARIES TRUE)

## In most cases better set in the CMake cache
# set(SFML_DIR "<sfml root prefix>/lib/cmake/SFML")

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_COMPILER g++)
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
add_executable(SFMLTest main.cpp grid.cpp application.cpp view.cpp additional.cpp)
target_link_libraries(SFMLTest sfml-graphics)

