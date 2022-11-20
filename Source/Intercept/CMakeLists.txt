file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

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

add_library(Intercept SHARED
    ${SourceFiles}
)

target_link_libraries(Intercept
    Detours
)

target_include_directories(Intercept PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${detours_SOURCE_DIR}/src
)
