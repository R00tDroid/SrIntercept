#pragma once
#include <d3d11.h>
#include <map>
#include <softcam.h>

class RenderContext
{
public:
    static RenderContext* GetContext(ID3D11RenderTargetView* targetView);

    void PreWeave();
    void PostWeave(unsigned int width, unsigned int height);

private:
    static std::map<ID3D11RenderTargetView*, RenderContext*> Instances;
    RenderContext(ID3D11RenderTargetView* targetView);

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    ID3D11RenderTargetView* targetView = nullptr;
    ID3D11Texture2D* targetTexture = nullptr;
    ID3D11ShaderResourceView* targetTextureSRV = nullptr;
    D3D11_TEXTURE2D_DESC targetDesc;
    ID3D11Texture2D* stagingTexture = nullptr;

    scCamera virtualWebcam = nullptr;
    unsigned char* outputBitmap = nullptr;
};
