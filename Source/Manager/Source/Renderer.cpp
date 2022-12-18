#include "App.hpp"
#include "Renderer.hpp"
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include "RenderContextProxy.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT MessageProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    ImGui_ImplWin32_WndProcHandler(handle, message, wParam, lParam);
    return DefWindowProcA(handle, message, wParam, lParam);
}

Renderer Renderer::instance;

bool Renderer::Init()
{
    if (!InitWindow()) return false;
    if (!InitD3D()) return false;
    if (!InitConverter()) return false;

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

    if (selectedRenderContext != -1)
    {
        D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (float)windowSize.x, (float)windowSize.y, 0.0f, 1.0f };
        d3dContext->RSSetViewports(1, &viewport);

        RenderContextProxy* proxy = renderContextProxies[selectedRenderContext];

        d3dContext->VSSetShader(conversionVS, nullptr, 0);
        d3dContext->PSSetShader(conversionPS, nullptr, 0);
        d3dContext->IASetInputLayout(conversionIL);

        d3dContext->PSSetShaderResources(0, 1, &proxy->framebufferView);

        UINT stride = sizeof(float) * 4;
        UINT offset = 0;
        d3dContext->IASetVertexBuffers(0, 1, &conversionGeometry, &stride, &offset);
        d3dContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        d3dContext->Draw(3, 0);
    }

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

    d3d_release(conversionGeometry);
    d3d_release(conversionVS);
    d3d_release(conversionPS);
    d3d_release(conversionIL);

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
    winClass.lpfnWndProc = MessageProc;
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

bool Renderer::InitConverter()
{
    cmrc::file fileVS = resourceFS->open("Source/Convert_vs.cso");
    d3dDevice->CreateVertexShader(fileVS.begin(), fileVS.size(), nullptr, &conversionVS);

    cmrc::file filePS = resourceFS->open("Source/Convert_ps.cso");
    d3dDevice->CreatePixelShader(filePS.begin(), filePS.size(), nullptr, &conversionPS);

    D3D11_INPUT_ELEMENT_DESC inputElements[] =
    {
        { "SV_POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    d3dDevice->CreateInputLayout(inputElements, _countof(inputElements), fileVS.begin(), fileVS.size(), &conversionIL);

    struct Vertex
    {
        float Position[2];
        float uv[2];
    };
    Vertex vertices[3] =
    {
        {{-1, 1}, {0, 0}},
        {{3, 1}, {2, 0}},
        {{-1, -3}, {0, 2}},
    };

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.ByteWidth = sizeof(vertices);
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexSubresourceData = { vertices };
    d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &conversionGeometry);

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
    if (selectedRenderContext >= renderContextProxies.size())
    {
        selectedRenderContext = -1;
    }
    if (selectedRenderContext == -1 && !renderContextProxies.empty())
    {
        selectedRenderContext = 0;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(float(windowSize.x), float(windowSize.y));

    ImGui_ImplDX11_NewFrame();
    ImGui::NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(120, io.DisplaySize.y - 20));
    if (ImGui::Begin("Input list"))
    {
        for (int i = 0; i < renderContextProxies.size(); i++)
        {
            RenderContextProxy* proxy = renderContextProxies[i];

            if (selectedRenderContext != i)
            {
                if (ImGui::Button("Activate"))
                {
                    selectedRenderContext = i;
                }
            }

            ImGui::Image(proxy->framebufferView, ImVec2(100, 50));
            ImGui::Spacing();
        }
    }
    ImGui::End();

    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
