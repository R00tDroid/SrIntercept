cmake_minimum_required(VERSION 3.11)
project(SrIntercept VERSION 0.0.1)

option(BUILD_TESTAPP "" ON)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include(CMake/CPM.cmake)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Source/Intercept)

if (BUILD_TESTAPP)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Source/TestApp)
endif()
