#include <windows.h>
#include <detours.h>
#include "HookUtils.hpp"

typedef void(__thiscall* DX11WeaverBase_Weave_t)(void* __this, unsigned int width, unsigned int height);
DX11WeaverBase_Weave_t DX11WeaverBase_Weave;

__declspec(dllexport) void __fastcall Override_DX11WeaverBase_Weave(void* __this, unsigned int width, unsigned int height)
{
    DX11WeaverBase_Weave(__this, width, height);
}

BOOL WINAPI DllMain(HINSTANCE, DWORD Event, LPVOID) 
{
    switch(Event)
    { 
        case DLL_PROCESS_ATTACH:
        {
            DetourRestoreAfterWith();
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            AttachFunction<DX11WeaverBase_Weave_t>("SimulatedRealityDirectX.dll", "SR::DX11WeaverBase::Weave", DX11WeaverBase_Weave, Override_DX11WeaverBase_Weave);
            DetourTransactionCommit();
            break;
        }

        case DLL_PROCESS_DETACH:
        {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            // Detach overrides
            DetourTransactionCommit();
            break;
        }
    }
    return TRUE;
}
