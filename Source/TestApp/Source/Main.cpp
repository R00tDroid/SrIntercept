#include <d3d11.h>
#include <vector>
#include <filesystem>
#include <DirectXMath.h>
#include <sr/management/srcontext.h>
#include <sr/weaver/dx11weaver.h>

ID3D11Device* d3dDevice = nullptr;
ID3D11DeviceContext* d3dContext = nullptr;

IDXGISwapChain* dxgiSwapchain = nullptr;
ID3D11RenderTargetView* backBufferView = nullptr;
HWND window = nullptr;

ID3D11VertexShader* vertexShader = nullptr;
ID3D11PixelShader* pixelShader = nullptr;
ID3D11InputLayout* inputLayout = nullptr;

std::filesystem::path appDirectory;

unsigned int renderWidth = 800;
unsigned int renderHeight = 600;

ID3D11Buffer* vertexBuffer = nullptr;

typedef std::chrono::system_clock clock_type;
clock_type::time_point lastUpdate;
float deltaTime = 0;

struct ConstantBuffer
{
    DirectX::XMMATRIX transform, projection;
};
ID3D11Buffer* constantBuffer = nullptr;

SR::SRContext* srContext = nullptr;
SR::DX11Weaver* srWeaver = nullptr;

typedef std::vector<unsigned char> BinaryBlob;
BinaryBlob LoadFile(std::filesystem::path filePath)
{
    FILE* file = nullptr;
    fopen_s(&file, filePath.string().c_str(), "rb");

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    BinaryBlob blob;
    blob.resize(fileSize);
    fread(blob.data(), sizeof(unsigned char), fileSize, file);

    fclose(file);

    return blob;
}

bool InitWindow()
{
    WNDCLASSEXA winClass = {};
    winClass.cbSize = sizeof(WNDCLASSEXA);
    winClass.style = CS_HREDRAW | CS_VREDRAW;
    winClass.lpfnWndProc = &DefWindowProc;
    winClass.hInstance = GetModuleHandleA(nullptr);
    winClass.lpszClassName = "WindowClass";

    RegisterClassExA(&winClass);

    window = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, winClass.lpszClassName, "", WS_OVERLAPPEDWINDOW | WS_VISIBLE,CW_USEDEFAULT, CW_USEDEFAULT, renderWidth, renderHeight, 0, 0, GetModuleHandleA(nullptr), 0);

    return true;
}

void UpdateWindow()
{
    MSG message;
    while (PeekMessageA(&message, window, 0, 0, true))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

bool InitPipeline()
{
    BinaryBlob vertexBlob = LoadFile(appDirectory / "Cube_vs.cso");
    BinaryBlob pixelBlob = LoadFile(appDirectory / "Cube_ps.cso");

    d3dDevice->CreateVertexShader(vertexBlob.data(), vertexBlob.size(), nullptr, &vertexShader);
    d3dDevice->CreatePixelShader(pixelBlob.data(), pixelBlob.size(), nullptr, &pixelShader);

    D3D11_INPUT_ELEMENT_DESC inputElements[] =
    {
        { "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    d3dDevice->CreateInputLayout(inputElements, _countof(inputElements), vertexBlob.data(), vertexBlob.size(), &inputLayout);

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(ConstantBuffer);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    d3dDevice->CreateBuffer(&desc, nullptr, &constantBuffer);

    return true;
}

// 6 Sides, 2 triangles per side, 3 vertices per triangle
#define VertexCount 6 * 2 * 3

#define CubeSide(va, vb, vc, vd) corners[va], corners[vb], corners[vc], corners[vc], corners[vd], corners[va]

bool InitMesh()
{
    struct Vertex
    {
        float Position[3];
        float Color[3];
    };

    Vertex corners[8] =
    {
        { {1, 1, 1}, {1, 0, 0} },
        { {1, -1, 1}, {0, 1, 0} },
        { {-1, -1, 1}, {0, 0, 1} },
        { {-1, 1, 1}, {1, 0, 1} },

        { {1, 1, -1}, {1, 1, 1} },
        { {1, -1, -1}, {1, 1, 1} },
        { {-1, -1, -1}, {1, 1, 1} },
        { {-1, 1, -1}, {1, 1, 1} },
    };

    Vertex triangles[VertexCount]
    {
        CubeSide(3, 2, 1, 0), // Front
        CubeSide(4, 5, 6, 7), // Back

        CubeSide(2, 3, 7, 6), // Left
        CubeSide(0, 1, 5, 4), // Right

        CubeSide(4, 7, 3, 0), // Top
        CubeSide(1, 2, 6, 5) // Bottom
    };

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.ByteWidth = sizeof(triangles);
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexSubresourceData = { triangles };

    d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertexBuffer);

    return true;
}

bool InitD3D()
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

    if (!InitPipeline()) return false;
    if (!InitMesh()) return false;

    return true;
}

DirectX::XMMATRIX CalculateProjection(DirectX::XMVECTOR eye, float screenWidth, float screenHeight, float fnear, float ffar)
{
    DirectX::XMVECTOR pa = { -screenWidth / 2, screenHeight / 2, 0 };
    DirectX::XMVECTOR pb = { screenWidth / 2, screenHeight / 2, 0 };
    DirectX::XMVECTOR pc = { -screenWidth / 2, -screenHeight / 2, 0 };

    DirectX::XMVECTOR vr = { 1, 0, 0 };
    DirectX::XMVECTOR vu = { 0, 1, 0 };
    DirectX::XMVECTOR vn = { 0, 0, 1 };

    DirectX::XMVECTOR va = DirectX::XMVectorSubtract(pa, eye);
    DirectX::XMVECTOR vb = DirectX::XMVectorSubtract(pb, eye);
    DirectX::XMVECTOR vc = DirectX::XMVectorSubtract(pc, eye);

    float screenDistance = -1 * DirectX::XMVector3Dot(va, vn).m128_f32[0];

    float left = DirectX::XMVector3Dot(vr, va).m128_f32[0] * fnear / screenDistance;
    float right = DirectX::XMVector3Dot(vr, vb).m128_f32[0] * fnear / screenDistance;
    float top = DirectX::XMVector3Dot(vu, va).m128_f32[0] * fnear / screenDistance;
    float bottom = DirectX::XMVector3Dot(vu, vc).m128_f32[0] * fnear / screenDistance;

    float m00 = (2 * fnear) / (right - left);
    float m11 = (2 * fnear) / (top - bottom);
    float m20 = (right + left) / (right - left);
    float m21 = (top + bottom) / (top - bottom);
    float m22 = -(ffar + fnear) / (ffar - fnear);
    float m23 = -1;
    float m32 = -2 * (ffar * fnear) / (ffar - fnear);

    DirectX::XMMATRIX frustum = DirectX::XMMATRIX(m00, 0, 0, 0, 0, m11, 0, 0, m20, m21, m22, m23, 0, 0, m32, 0);
    DirectX::XMMATRIX eyeOffset = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorScale(eye, -1));
    return DirectX::XMMatrixMultiply(eyeOffset, frustum);
}

float r = 0;

void RenderFrame()
{
    ID3D11RenderTargetView* frameBuffer = srWeaver->getFrameBuffer();
    d3dContext->OMSetRenderTargets(1, &frameBuffer, nullptr);

    float backgroundColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
    d3dContext->ClearRenderTargetView(frameBuffer, backgroundColor);

    d3dContext->IASetInputLayout(inputLayout);
    d3dContext->VSSetShader(vertexShader, nullptr, 0);
    d3dContext->PSSetShader(pixelShader, nullptr, 0);

    UINT stride = sizeof(float) * 6;
    UINT offset = 0;
    d3dContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    d3dContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    d3dContext->VSSetConstantBuffers(0, 1, &constantBuffer);

    float cubeSize = 10;
    r += 90.0f * deltaTime;

    DirectX::XMVECTOR eyes[2] = 
    {
        {-3, 0, 60},
        {3, 0, 60}
    };

    for (int i = 0; i < 2; i++)
    {
        D3D11_VIEWPORT viewport = { (float)(i * renderWidth / 2), 0.0f, (float)renderWidth / 2, (float)renderHeight / 2, 0.0f, 1.0f };
        d3dContext->RSSetViewports(1, &viewport);

        D3D11_MAPPED_SUBRESOURCE constantBufferMapping;
        d3dContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantBufferMapping);
        ConstantBuffer* constantBufferData = static_cast<ConstantBuffer*>(constantBufferMapping.pData);
        constantBufferData->transform = DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(cubeSize, cubeSize, cubeSize) * DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(r)));
        constantBufferData->projection = DirectX::XMMatrixTranspose(CalculateProjection(eyes[i], 69, 39, 0.1f, 200.0f));
        d3dContext->Unmap(constantBuffer, 0);

        d3dContext->Draw(VertexCount, 0);
    }

    d3dContext->OMSetRenderTargets(1, &backBufferView, nullptr);

    D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (float)renderWidth, (float)renderHeight, 0.0f, 1.0f };
    d3dContext->RSSetViewports(1, &viewport);

    srWeaver->weave(renderWidth, renderHeight);

    dxgiSwapchain->Present(0, 0);
}

bool InitSR()
{
    srContext = SR::SRContext::create();
    srWeaver = new SR::DX11Weaver(*srContext, d3dDevice, d3dContext, renderWidth, renderHeight / 2, window);
    srContext->initialize();
    return true;
}

int main()
{
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);

    appDirectory = std::filesystem::path(std::string(exePath)).parent_path();

    if (!InitWindow()) return -1;
    if (!InitD3D()) return -1;
    if (!InitSR()) return -1;

    lastUpdate = clock_type::now();

    while (true)
    {
        clock_type::time_point now = clock_type::now();
        clock_type::duration delta = now - lastUpdate;
        deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count() / 1000.0f;
        lastUpdate = now;

        UpdateWindow();
        RenderFrame();
    }

    return 0;
}
