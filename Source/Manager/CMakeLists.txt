file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/Source/Convert_vs.hlsl PROPERTIES VS_SHADER_TYPE Vertex VS_SHADER_MODEL 5.0 VS_SHADER_OBJECT_FILE_NAME "${CMAKE_CURRENT_LIST_DIR}/Source/Convert_vs.cso")
set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/Source/Convert_ps.hlsl PROPERTIES VS_SHADER_TYPE Pixel VS_SHADER_MODEL 5.0 VS_SHADER_OBJECT_FILE_NAME "${CMAKE_CURRENT_LIST_DIR}/Source/Convert_ps.cso")

add_executable(Manager WIN32
    ${SourceFiles}
)

target_link_libraries(Manager
    Detours
    Shared
    Softcam
    ImGui
    d3d11
)

target_include_directories(Manager PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${detours_SOURCE_DIR}/src
)

set_target_properties(Manager PROPERTIES FOLDER "SrIntercept")

target_compile_features(Manager PRIVATE cxx_std_17)

add_dependencies(Manager Intercept)

set_target_properties(Manager PROPERTIES OUTPUT_NAME "SrIntercept")
