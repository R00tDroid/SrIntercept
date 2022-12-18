#include "Renderer.hpp"
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include "RenderContextProxy.hpp"

Renderer Renderer::instance;

bool Renderer::Init()
{
    if (!InitWindow()) return false;
    if (!InitD3D()) return false;

    ImGui::CreateContext();
    ImGui_ImplDX11_Init(d3dDevice, d3dContext);
    ImGui_ImplWin32_Init(window);

    return true;
}

void Renderer::Render()
{
    UpdateWindow();

    for (RenderContextProxy* proxy : renderContextProxies)
    {
        proxy->UpdateFramebuffer();
    }

    d3dContext->OMSetRenderTargets(1, &backBufferView, nullptr);

    float backgroundColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
    d3dContext->ClearRenderTargetView(backBufferView, backgroundColor);

    RenderUI();

    dxgiSwapchain->Present(1, 0);
}

void Renderer::Destroy()
{
    ImGui_ImplWin32_Shutdown();
    ImGui_ImplDX11_Shutdown();
    ImGui::DestroyContext();

    if (window != nullptr)
    {
        DestroyWindow(window);
        window = nullptr;
    }

    d3d_release(backBufferView);
    d3d_release(dxgiSwapchain);
    d3d_release(d3dContext);
    d3d_release(d3dDevice);
}

bool Renderer::InitWindow()
{
    WNDCLASSEXA winClass = {};
    winClass.cbSize = sizeof(WNDCLASSEXA);
    winClass.style = CS_HREDRAW | CS_VREDRAW;
    winClass.lpfnWndProc = &DefWindowProc;
    winClass.hInstance = GetModuleHandleA(nullptr);
    winClass.lpszClassName = "WindowClass";

    RegisterClassExA(&winClass);

    window = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, winClass.lpszClassName, "", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, windowSize.x, windowSize.y, 0, 0, GetModuleHandleA(nullptr), 0);

    return true;
}

bool Renderer::InitD3D()
{
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    UINT creationFlags = 0;
#ifndef _NDEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DXGI_SWAP_CHAIN_DESC swapchainDesc;
    ZeroMemory(&swapchainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapchainDesc.Windowed = TRUE;
    swapchainDesc.BufferCount = 2;
    swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchainDesc.SampleDesc.Count = 1;
    swapchainDesc.SampleDesc.Quality = 0;
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapchainDesc.OutputWindow = window;

    D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &swapchainDesc, &dxgiSwapchain, &d3dDevice, nullptr, &d3dContext);

    ID3D11Texture2D* backBuffer;
    dxgiSwapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    d3dDevice->CreateRenderTargetView(backBuffer, 0, &backBufferView);

    return true;
}

void Renderer::UpdateWindow()
{
    MSG message;
    while (PeekMessageA(&message, window, 0, 0, true))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

void Renderer::RenderUI()
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(float(windowSize.x), float(windowSize.y));

    ImGui_ImplDX11_NewFrame();
    ImGui::NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::SetNextWindowSize(ImVec2(300, 250));
    if (ImGui::Begin("Sr Intercept"))
    {
        for (RenderContextProxy* proxy : renderContextProxies)
        {
            ImGui::Image(proxy->framebufferView, ImVec2(100, 50));
        }

        ImGui::End();
    }

    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
