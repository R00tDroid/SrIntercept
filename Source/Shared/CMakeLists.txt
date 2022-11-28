file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_library(Shared STATIC
    ${SourceFiles}
)

target_link_libraries(Shared
    Detours
)

target_include_directories(Shared PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${detours_SOURCE_DIR}/src
)

set_target_properties(Shared PROPERTIES FOLDER "SrIntercept")

target_compile_features(Shared PRIVATE cxx_std_17)

