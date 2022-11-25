#include <d3d11.h>
#include <windows.h>
#include <detours.h>
#include "HookUtils.hpp"
#include "RenderContext.hpp"

typedef void(__thiscall* DX11WeaverBase_weave_t)(void* object, unsigned int width, unsigned int height);
DX11WeaverBase_weave_t DX11WeaverBase_weave;

typedef ID3D11RenderTargetView*(__thiscall* DX11WeaverBase_getFrameBuffer_t)(void* object);
DX11WeaverBase_getFrameBuffer_t DX11WeaverBase_getFrameBuffer;

__declspec(dllexport) void __fastcall Override_DX11WeaverBase_weave(void* object, unsigned int width, unsigned int height)
{
    RenderContext* context = nullptr;
    ID3D11RenderTargetView* frameBuffer = DX11WeaverBase_getFrameBuffer(object);
    if (frameBuffer != nullptr)
    {
        context = RenderContext::GetContext(frameBuffer);
        context->PreWeave();
    }
    DX11WeaverBase_weave(object, width, height);
    if (context != nullptr)
    {
        context->PostWeave(width, height);
    }
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

            FunctionPointer weave = FindFunction("SimulatedRealityDirectX.dll", "?weave@DX11WeaverBase@SR@@QEAAXII@Z");
            if (weave == nullptr)
            {
                MessageBoxA(nullptr, "Failed to hook SR::DX11WeaverBase::weave", "", MB_OK + MB_ICONERROR);
                exit(-1);
            }

            FunctionPointer getFrameBuffer = FindFunction("SimulatedRealityDirectX.dll", "?getFrameBuffer@DX11WeaverBase@SR@@QEAAPEAUID3D11RenderTargetView@@XZ");
            if (getFrameBuffer == nullptr)
            {
                MessageBoxA(nullptr, "Failed to hook SR::DX11WeaverBase::getFrameBuffer", "", MB_OK + MB_ICONERROR);
                exit(-1);
            }
            DX11WeaverBase_getFrameBuffer = static_cast<DX11WeaverBase_getFrameBuffer_t>(getFrameBuffer);

            AttachFunction<DX11WeaverBase_weave_t>(weave, DX11WeaverBase_weave, Override_DX11WeaverBase_weave);
            DetourTransactionCommit();

            HMODULE softcamModule = GetModuleHandleA("softcam.dll");
            auto DllRegisterServer = GetProcAddress(softcamModule, "DllRegisterServer");
            HRESULT result = DllRegisterServer();
            if (FAILED(result))
            {
                MessageBoxA(nullptr, "Failed to call DllRegisterServer", "", MB_OK + MB_ICONERROR);
                exit(-1);
            }

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
