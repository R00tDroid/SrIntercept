CPMAddPackage("gh:microsoft/detours#v4.0.1")

set(DetoursSource
    ${detours_SOURCE_DIR}/src/detours.h
    ${detours_SOURCE_DIR}/src/detours.cpp
    ${detours_SOURCE_DIR}/src/disasm.cpp
    ${detours_SOURCE_DIR}/src/modules.cpp
    ${detours_SOURCE_DIR}/src/creatwth.cpp
)

add_library(Detours STATIC
    ${DetoursSource}
)
set_target_properties(Detours PROPERTIES FOLDER "Dependencies")

CPMAddPackage(
    NAME clsocket
    GITHUB_REPOSITORY DFHack/clsocket
    GIT_TAG origin/master
    DOWNLOAD_ONLY TRUE
)

set(ClsocketSource
    ${clsocket_SOURCE_DIR}/src/ActiveSocket.h
    ${clsocket_SOURCE_DIR}/src/ActiveSocket.cpp
    ${clsocket_SOURCE_DIR}/src/Host.h
    ${clsocket_SOURCE_DIR}/src/PassiveSocket.h
    ${clsocket_SOURCE_DIR}/src/PassiveSocket.cpp
    ${clsocket_SOURCE_DIR}/src/SimpleSocket.h
    ${clsocket_SOURCE_DIR}/src/SimpleSocket.cpp
    ${clsocket_SOURCE_DIR}/src/statTimer.h
)

add_library(clsocket STATIC
    ${ClsocketSource}
)
target_compile_options(clsocket PUBLIC /WX- /W1)
set_target_properties(clsocket PROPERTIES FOLDER "Dependencies")

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
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
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
