PROJECT(amethyst)
cmake_minimum_required(VERSION 3.4.1)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

include(cmake/utils.cmake)

enable_testing()
add_subdirectory(src)
