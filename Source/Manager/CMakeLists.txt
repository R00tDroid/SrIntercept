file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_executable(Manager WIN32
    ${SourceFiles}
)

target_link_libraries(Manager
    Detours
)

target_include_directories(Manager PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${detours_SOURCE_DIR}/src
)

set_target_properties(Manager PROPERTIES FOLDER "SrIntercept")

target_compile_features(Manager PRIVATE cxx_std_17)

add_dependencies(Manager Intercept)

set_target_properties(Manager PROPERTIES OUTPUT_NAME "SrIntercept")
