#include "ClientMessaging.hpp"
#include "RenderContext.hpp"
#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "InterceptLog.hpp"

std::map<ID3D11RenderTargetView*, RenderContext*> RenderContext::instances;

RenderContext* RenderContext::GetContext(ID3D11RenderTargetView* targetView)
{
    auto it = instances.find(targetView);
    if (it != instances.end())
    {
        return it->second;
    }

    RenderContext* newContext = new RenderContext(targetView);
    instances.insert(std::pair<ID3D11RenderTargetView*, RenderContext*>(targetView, newContext));
    return newContext;
}

void RenderContext::PreWeave()
{
    if (sharedTexture != nullptr && sharedTextureLock != nullptr)
    {
        sharedTextureLock->AcquireSync(0, 100);
        context->CopyResource(sharedTexture, targetTexture);
        sharedTextureLock->ReleaseSync(0);
    }
}

void RenderContext::PostWeave(unsigned width, unsigned height)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(float(width), float(height));

    ImGui_ImplDX11_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(300, 250));
    if (ImGui::Begin("Sr Intercept"))
    {
        ImGui::Text("0x%x", targetView);
        ImGui::Image(targetTextureSRV, ImVec2(280, 190));
        ImGui::End();
    }

    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

HANDLE RenderContext::GetShareHandle(DWORD processId)
{
    auto it = shareHandles.find(processId);
    if (it == shareHandles.end())
    {
        HANDLE processHandle = OpenProcess(PROCESS_DUP_HANDLE, FALSE, processId);
        HANDLE processShareHandle = nullptr;
        DuplicateHandle(GetCurrentProcess(), sharedTextureHandle, processHandle, &processShareHandle, 0, false, DUPLICATE_SAME_ACCESS);
        CloseHandle(processHandle);
        shareHandles.insert(std::pair<DWORD, HANDLE>(processId, processShareHandle));
    }

    return shareHandles[processId];
}

RenderContext::RenderContext(ID3D11RenderTargetView* inTargetView) : targetView(inTargetView)
{
    targetView->GetResource(reinterpret_cast<ID3D11Resource**>(&targetTexture));

    targetTexture->GetDesc(&targetDesc);

    targetTexture->GetDevice(&device);
    device->GetImmediateContext(&context);

    device->CreateShaderResourceView(targetTexture, nullptr, &targetTextureSRV);

    ImGui::CreateContext();
    ImGui_ImplDX11_Init(device, context);

    D3D11_TEXTURE2D_DESC sharedTexDesc = targetDesc;
    sharedTexDesc.BindFlags = 0;
    sharedTexDesc.CPUAccessFlags = 0;
    sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_NTHANDLE | D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

    if (FAILED(device->CreateTexture2D(&sharedTexDesc, nullptr, &sharedTexture)))
    {
        logger.Log("Failed to create shared texture");
    }
    else
    {
        sharedTexture->QueryInterface(IID_PPV_ARGS(&sharedTextureLock));

        IDXGIResource1* sharedResource = nullptr;
        sharedTexture->QueryInterface(IID_PPV_ARGS(&sharedResource));
        sharedResource->CreateSharedHandle(nullptr, DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE, nullptr, &sharedTextureHandle);
        sharedResource->Release();
    }

    ClientMessaging::instance.SendRenderContextInfo(this);
}
