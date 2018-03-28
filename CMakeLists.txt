PROJECT(amethyst)
cmake_minimum_required(VERSION 3.4.1)

include(cmake/utils.cmake)

enable_testing()
add_subdirectory(src)
add_subdirectory(test)
