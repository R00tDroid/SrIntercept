file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

find_package(simulatedreality REQUIRED)
find_package(srDirectX REQUIRED)

add_executable(TestApp
    ${SourceFiles}
)

target_link_libraries(TestApp PRIVATE
    simulatedreality
    srDirectX::srDirectX
    d3d11
)
