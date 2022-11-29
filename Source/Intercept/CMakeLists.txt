file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_library(Intercept SHARED
    ${SourceFiles}
)

target_link_libraries(Intercept
    Detours
    ImGui
    Softcam
    Shared
)

target_include_directories(Intercept PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${detours_SOURCE_DIR}/src
)

set_target_properties(Intercept PROPERTIES FOLDER "SrIntercept")

set_target_properties(Intercept PROPERTIES OUTPUT_NAME "SrIntercept")
