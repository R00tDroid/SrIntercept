#pragma once
#include <d3d11_1.h>
#include <map>

class RenderContext
{
public:
    static RenderContext* GetContext(ID3D11RenderTargetView* targetView);

    void PreWeave();
    void PostWeave(unsigned int width, unsigned int height);

    static std::map<ID3D11RenderTargetView*, RenderContext*> instances;

    HANDLE GetShareHandle(DWORD processId);

private:
    RenderContext(ID3D11RenderTargetView* targetView);

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    ID3D11RenderTargetView* targetView = nullptr;
    ID3D11Texture2D* targetTexture = nullptr;
    ID3D11ShaderResourceView* targetTextureSRV = nullptr;
    D3D11_TEXTURE2D_DESC targetDesc;

    HANDLE sharedTextureHandle = nullptr;
    ID3D11Texture2D* sharedTexture = nullptr;
    IDXGIKeyedMutex* sharedTextureLock = nullptr;

    std::map<DWORD, HANDLE> shareHandles;
};
