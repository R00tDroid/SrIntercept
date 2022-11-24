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

void RenderContext::Render()
{
    ID3D11RenderTargetView* restoreView = nullptr;
    context->OMGetRenderTargets(1, &restoreView, nullptr);
    context->OMSetRenderTargets(1, &targetView, nullptr);

    ImGui_ImplDX11_NewFrame();
    ImGui::NewFrame();

    if (ImGui::Begin("Sr Intercept"))
    {
        ImGui::Text("0x%x", targetView);
        ImGui::End();
    }

    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (restoreView != nullptr) 
    {
        context->OMSetRenderTargets(1, &restoreView, nullptr);
        restoreView->Release();
    }
}

RenderContext::RenderContext(ID3D11RenderTargetView* inTargetView) : targetView(inTargetView)
{
    ID3D11Texture2D* Texture = nullptr;
    targetView->GetResource(reinterpret_cast<ID3D11Resource**>(&Texture));

    Texture->GetDesc(&targetDesc);

    Texture->GetDevice(&device);
    device->GetImmediateContext(&context);

    Texture->Release();

    ImGui::CreateContext();
    ImGui_ImplDX11_Init(device, context);
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(float(targetDesc.Width / 2), float(targetDesc.Height));
}
