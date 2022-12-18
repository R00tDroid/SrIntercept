#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#define d3d_release(x) if (x != nullptr) { x->Release(); x = nullptr; }

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

private:
    bool InitWindow();
    bool InitD3D();
    void UpdateWindow();

    DirectX::XMINT2 windowSize = { 800, 600 };
};