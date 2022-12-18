file(GLOB_RECURSE ShaderFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.hlsl   
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.hlsli
)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.hpp
)

set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/Source/Convert_vs.hlsl PROPERTIES VS_SHADER_TYPE Vertex VS_SHADER_MODEL 5.0 VS_SHADER_OBJECT_FILE_NAME "${CMAKE_CURRENT_LIST_DIR}/Source/Convert_vs.cso")
set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/Source/Convert_ps.hlsl PROPERTIES VS_SHADER_TYPE Pixel VS_SHADER_MODEL 5.0 VS_SHADER_OBJECT_FILE_NAME "${CMAKE_CURRENT_LIST_DIR}/Source/Convert_ps.cso")

set(ResourceFiles
    "${CMAKE_CURRENT_LIST_DIR}/Source/Convert_vs.cso"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Convert_ps.cso"  
)
cmrc_add_resource_library(ManagerResources ALIAS Manager::rc ${ResourceFiles})
set_target_properties(ManagerResources PROPERTIES FOLDER "Dependencies")

add_library(ManagerShaders SHARED
    ${ShaderFiles}
)
set_target_properties(ManagerShaders PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(ManagerShaders PROPERTIES FOLDER "SrIntercept")

add_executable(Manager WIN32
    ${SourceFiles}
)

target_link_libraries(Manager
    Detours
    Shared
    Softcam
    ImGui
    d3d11
    Manager::rc
)

target_include_directories(Manager PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${detours_SOURCE_DIR}/src
)

set_target_properties(Manager PROPERTIES FOLDER "SrIntercept")

target_compile_features(Manager PRIVATE cxx_std_17)

add_dependencies(Manager Intercept)

set_target_properties(Manager PROPERTIES OUTPUT_NAME "SrIntercept")

add_dependencies(ManagerResources ManagerShaders)
add_dependencies(Manager ManagerResources)
