file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_executable(Injector WIN32
    ${SourceFiles}
)

target_link_libraries(Injector
    Detours
)

target_include_directories(Injector PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${detours_SOURCE_DIR}/src
)

set_target_properties(Injector PROPERTIES FOLDER "SrIntercept")

add_dependencies(Injector Intercept)

set_target_properties(Injector PROPERTIES OUTPUT_NAME "SrIntercept")
