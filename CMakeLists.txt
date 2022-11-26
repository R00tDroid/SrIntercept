cmake_minimum_required(VERSION 3.11)
project(SrIntercept VERSION 0.0.1)

option(BUILD_TESTAPP "" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

foreach(ConfigName ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${ConfigName} ConfigName)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${ConfigName} "${CMAKE_CURRENT_LIST_DIR}/Bin" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${ConfigName} "${CMAKE_CURRENT_LIST_DIR}/Bin" CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${ConfigName} "${CMAKE_CURRENT_LIST_DIR}/Bin" CACHE PATH "" FORCE)
endforeach()

include(CMake/get_cpm.cmake)

CPMAddPackage("gh:microsoft/detours#v4.0.1")

set(DetoursSource
    ${detours_SOURCE_DIR}/src/detours.h
    ${detours_SOURCE_DIR}/src/detours.cpp
    ${detours_SOURCE_DIR}/src/disasm.cpp
    ${detours_SOURCE_DIR}/src/modules.cpp
    ${detours_SOURCE_DIR}/src/creatwth.cpp
)

add_library(Detours STATIC
    ${DetoursSource}
)
set_target_properties(Detours PROPERTIES FOLDER "Dependencies")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Source/Intercept)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Source/Manager)

if (BUILD_TESTAPP)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Source/TestApp)
endif()
