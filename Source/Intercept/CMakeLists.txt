file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

CPMAddPackage("gh:ocornut/imgui#v1.89")

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

add_library(Intercept SHARED
    ${SourceFiles}
)

target_link_libraries(Intercept
    Detours
    ImGui
)

target_include_directories(Intercept PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${detours_SOURCE_DIR}/src
)
