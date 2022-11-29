#include <d3d11.h>
#include <windows.h>
#include <detours.h>
#include "InterceptLog.hpp"
#include "HookUtils.hpp"
#include "RenderContext.hpp"

Logger logger;

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
            logger.Log("Process attached");

            DetourRestoreAfterWith();
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());

            FunctionPointer weave = FindFunction("SimulatedRealityDirectX.dll", "?weave@DX11WeaverBase@SR@@QEAAXII@Z");
            if (weave == nullptr)
            {
                logger.Log("Failed to hook SR::DX11WeaverBase::weave");
                MessageBoxA(nullptr, "Failed to hook SR::DX11WeaverBase::weave", "", MB_OK + MB_ICONERROR);
                exit(-1);
            }
            else
            {
                logger.Log("SR::DX11WeaverBase::weave: 0x%x", weave);
            }

            FunctionPointer getFrameBuffer = FindFunction("SimulatedRealityDirectX.dll", "?getFrameBuffer@DX11WeaverBase@SR@@QEAAPEAUID3D11RenderTargetView@@XZ");
            if (getFrameBuffer == nullptr)
            {
                logger.Log("Failed to hook SR::DX11WeaverBase::getFrameBuffer");
                MessageBoxA(nullptr, "Failed to hook SR::DX11WeaverBase::getFrameBuffer", "", MB_OK + MB_ICONERROR);
                exit(-1);
            }
            else
            {
                logger.Log("SR::DX11WeaverBase::getFrameBuffer: 0x%x", getFrameBuffer);
            }

            DX11WeaverBase_getFrameBuffer = static_cast<DX11WeaverBase_getFrameBuffer_t>(getFrameBuffer);
            AttachFunction<DX11WeaverBase_weave_t>(weave, DX11WeaverBase_weave, Override_DX11WeaverBase_weave);
            DetourTransactionCommit();

            break;
        }

        case DLL_PROCESS_DETACH:
        {
            logger.Log("Process detach");

            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            // Detach overrides
            DetourTransactionCommit();
            break;
        }
    }
    return TRUE;
}
