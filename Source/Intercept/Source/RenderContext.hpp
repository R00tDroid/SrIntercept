#pragma once
#include <d3d11.h>
#include <map>

class RenderContext
{
public:
    static RenderContext* GetContext(ID3D11RenderTargetView* targetView);

    void Render();

private:
    static std::map<ID3D11RenderTargetView*, RenderContext*> Instances;
    RenderContext(ID3D11RenderTargetView* targetView);

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    ID3D11RenderTargetView* targetView = nullptr;
    D3D11_TEXTURE2D_DESC targetDesc;
};
