#pragma once
#include <d3d11_1.h>

class RenderContextProxy
{
public:
    RenderContextProxy(HANDLE shareHandle);

    void UpdateFramebuffer();

    ID3D11Texture2D* framebuffer = nullptr;
    ID3D11ShaderResourceView* framebufferView = nullptr;

private:
    ID3D11Texture2D* shareTexture = nullptr;
    IDXGIKeyedMutex* sharedTextureMutex = nullptr;
};
