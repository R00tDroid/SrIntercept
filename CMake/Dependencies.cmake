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
