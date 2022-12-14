file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

find_package(simulatedreality REQUIRED)
find_package(srDirectX REQUIRED)

set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/Source/Cube_vs.hlsl" PROPERTIES VS_SHADER_TYPE Vertex VS_SHADER_MODEL 5.0)
set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/Source/Cube_ps.hlsl" PROPERTIES VS_SHADER_TYPE Pixel VS_SHADER_MODEL 5.0)

add_executable(TestApp
    ${SourceFiles}
)

target_compile_features(TestApp PRIVATE cxx_std_17)

target_link_libraries(TestApp PRIVATE
    simulatedreality
    srDirectX::srDirectX
    d3d11
)

set_target_properties(TestApp PROPERTIES 
    FOLDER "Tests"
    VS_DEBUGGER_COMMAND "$<TARGET_FILE:Manager>"
    VS_DEBUGGER_COMMAND_ARGUMENTS "$<TARGET_FILE:TestApp>"
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Manager>"
)

add_dependencies(TestApp Manager)
