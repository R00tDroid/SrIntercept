#include "RenderContextProxy.hpp"

#include "Renderer.hpp"

RenderContextProxy::RenderContextProxy(HANDLE shareHandle)
{
    ID3D11Device1* device = nullptr;
    Renderer::instance.d3dDevice->QueryInterface(&device);

    device->OpenSharedResource1(shareHandle, IID_PPV_ARGS(&shareTexture));

    device->Release();

    shareTexture->QueryInterface(&sharedTextureMutex);

    D3D11_TEXTURE2D_DESC desc;
    shareTexture->GetDesc(&desc);

    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.MiscFlags = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;

    Renderer::instance.d3dDevice->CreateTexture2D(&desc, nullptr, &framebuffer);
    Renderer::instance.d3dDevice->CreateShaderResourceView(framebuffer, nullptr, &framebufferView);
}

void RenderContextProxy::UpdateFramebuffer()
{
    sharedTextureMutex->AcquireSync(0, 100);
    Renderer::instance.d3dContext->CopyResource(framebuffer, shareTexture);
    sharedTextureMutex->ReleaseSync(0);
}
