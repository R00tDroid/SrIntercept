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
}
