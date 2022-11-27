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
