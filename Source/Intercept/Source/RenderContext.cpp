#include "RenderContext.hpp"
#include "imgui.h"
#include "backends/imgui_impl_dx11.h"

std::map<ID3D11RenderTargetView*, RenderContext*> RenderContext::Instances;

RenderContext* RenderContext::GetContext(ID3D11RenderTargetView* targetView)
{
    auto it = Instances.find(targetView);
    if (it != Instances.end())
    {
        return it->second;
    }

    RenderContext* newContext = new RenderContext(targetView);
    Instances.insert(std::pair<ID3D11RenderTargetView*, RenderContext*>(targetView, newContext));
    return newContext;
}

void RenderContext::PreWeave()
{
    context->CopyResource(stagingTexture, targetTexture);

    D3D11_MAPPED_SUBRESOURCE mapDesc;
    context->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mapDesc);

    for (int y = 0; y < 300; y++)
    {
        for (int x = 0; x < 800; x++)
        {
            memcpy(&outputBitmap[(y * 800 + x) * 3], &static_cast<unsigned char*>(mapDesc.pData)[(y * 800 + x) * 4], 3);
        }
    }

    context->Unmap(stagingTexture, 0);

    if (virtualWebcam != nullptr)
    {
        scSendFrame(virtualWebcam, outputBitmap);
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

RenderContext::RenderContext(ID3D11RenderTargetView* inTargetView) : targetView(inTargetView)
{
    targetView->GetResource(reinterpret_cast<ID3D11Resource**>(&targetTexture));

    targetTexture->GetDesc(&targetDesc);

    targetTexture->GetDevice(&device);
    device->GetImmediateContext(&context);

    device->CreateShaderResourceView(targetTexture, nullptr, &targetTextureSRV);

    ImGui::CreateContext();
    ImGui_ImplDX11_Init(device, context);

    virtualWebcam = scCreateCamera(800, 300, 0);

    outputBitmap = (unsigned char*)malloc(800 * 300 * 3);

    D3D11_TEXTURE2D_DESC stagingDesc = targetDesc;
    stagingDesc.BindFlags = 0;
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    device->CreateTexture2D(&stagingDesc, nullptr, &stagingTexture);
}
