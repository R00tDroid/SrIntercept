#include "RenderContext.hpp"

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
}
