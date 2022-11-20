file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

CPMAddPackage("gh:microsoft/detours#v4.0.1")

file(GLOB_RECURSE DetoursSource
    ${detours_SOURCE_DIR}/src/*.*
)

add_library(Detours STATIC
    ${DetoursSource}
)

add_library(Intecept SHARED
    ${SourceFiles}
)

target_link_libraries(Intecept
    Detours
)
