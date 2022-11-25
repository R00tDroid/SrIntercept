file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

CPMAddPackage("gh:ocornut/imgui#v1.89")
CPMAddPackage("gh:tshino/softcam#v1.3")

set(ImGuiSource
    ${imgui_SOURCE_DIR}/imgui.h
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_internal.h
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)

add_library(ImGui STATIC
    ${ImGuiSource}
)
target_include_directories(ImGui PUBLIC
    ${imgui_SOURCE_DIR}
)

set_target_properties(ImGui PROPERTIES FOLDER "Dependencies")

file(GLOB_RECURSE SoftcamSource
    ${softcam_SOURCE_DIR}/src/*.cpp
    ${softcam_SOURCE_DIR}/src/*.h
    ${softcam_SOURCE_DIR}/src/*.def
)
add_library(Softcam SHARED
    ${SoftcamSource}
)
target_include_directories(Softcam PUBLIC
    ${softcam_SOURCE_DIR}/src
    ${softcam_SOURCE_DIR}/src/baseclasses
    ${softcam_SOURCE_DIR}/src/softcam
    ${softcam_SOURCE_DIR}/src/softcamcore
)
target_compile_options(Softcam PUBLIC /WX- /W1)
target_link_libraries(Softcam PUBLIC
    strmiids.lib
    winmm.lib
)

set_target_properties(Softcam PROPERTIES FOLDER "Dependencies")

add_library(Intercept SHARED
    ${SourceFiles}
)

target_link_libraries(Intercept
    Detours
    ImGui
    Softcam
)

target_include_directories(Intercept PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${detours_SOURCE_DIR}/src
)

set_target_properties(Intercept PROPERTIES FOLDER "SrIntercept")
