file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
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
