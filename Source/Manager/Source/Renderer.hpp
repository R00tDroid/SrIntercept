#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#define d3d_release(x) if (x != nullptr) { x->Release(); x = nullptr; }

class RenderContextProxy;

class Renderer
{
public:
    static Renderer instance;

    bool Init();
    void Render();
    void Destroy();

    ID3D11Device* d3dDevice = nullptr;
    ID3D11DeviceContext* d3dContext = nullptr;

    IDXGISwapChain* dxgiSwapchain = nullptr;
    ID3D11RenderTargetView* backBufferView = nullptr;

    HWND window = nullptr;

    std::vector<RenderContextProxy*> renderContextProxies;

private:
    bool InitWindow();
    bool InitD3D();
    bool InitConverter();
    void UpdateWindow();

    void RenderUI();

    DirectX::XMINT2 windowSize = { 800, 600 };

    int selectedRenderContext = -1;

    ID3D11VertexShader* conversionVS = nullptr;
    ID3D11PixelShader* conversionPS = nullptr;
    ID3D11InputLayout* conversionIL = nullptr;
    ID3D11Buffer* conversionGeometry = nullptr;
};
