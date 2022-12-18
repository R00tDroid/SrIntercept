#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#define d3d_release(x) if (x != nullptr) { x->Release(); x = nullptr; }

class RenderContextProxy;

enum ConversionMode
{
    CM_2D = 1,
    CM_Anaglyph = 2,
    CM_SBS = 3
};

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

    ConversionMode conversionMode = CM_SBS;

    int selectedRenderContext = -1;

    ID3D11VertexShader* conversionVS = nullptr;
    ID3D11PixelShader* conversionPS = nullptr;
    ID3D11InputLayout* conversionIL = nullptr;
    ID3D11Buffer* conversionGeometry = nullptr;
    ID3D11Buffer* conversionConstants = nullptr;

    DirectX::XMINT2 conversionResolution = { 3840, 1080 };

    ID3D11Texture2D* conversionTarget = nullptr;
    ID3D11ShaderResourceView* conversionTargetResource = nullptr;
    ID3D11RenderTargetView* conversionTargetView = nullptr;

    ID3D11Texture2D* webcamOutput = nullptr;
};
