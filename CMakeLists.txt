cmake_minimum_required(VERSION 3.11)
project(SrIntercept VERSION 0.0.1)

option(BUILD_TESTAPP "" ON)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include(CMake/CPM.cmake)

CPMAddPackage("gh:microsoft/detours#v4.0.1")

set(DetoursSource
    ${detours_SOURCE_DIR}/src/detours.h
    ${detours_SOURCE_DIR}/src/detours.cpp
    ${detours_SOURCE_DIR}/src/disasm.cpp
    ${detours_SOURCE_DIR}/src/modules.cpp
)

add_library(Detours STATIC
    ${DetoursSource}
)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Source/Intercept)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Source/Injector)

if (BUILD_TESTAPP)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Source/TestApp)
endif()
