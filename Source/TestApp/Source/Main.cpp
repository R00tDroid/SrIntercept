#include <d3d11.h>
#include <vector>
#include <filesystem>

ID3D11Device* d3dDevice = nullptr;
ID3D11DeviceContext* d3dContext = nullptr;

IDXGISwapChain* dxgiSwapchain = nullptr;
ID3D11RenderTargetView* backBufferView = nullptr;
HWND window = nullptr;

ID3D11VertexShader* vertexShader = nullptr;
ID3D11PixelShader* pixelShader = nullptr;
ID3D11InputLayout* inputLayout = nullptr;

std::filesystem::path appDirectory;

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

    window = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, winClass.lpszClassName, "", WS_OVERLAPPEDWINDOW | WS_VISIBLE,CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, 0, GetModuleHandleA(nullptr), 0);

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
        { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    d3dDevice->CreateInputLayout(inputElements, _countof(inputElements), vertexBlob.data(), vertexBlob.size(), &inputLayout);

    return true;
}

bool InitD3D()
{
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    UINT creationFlags = 0;
#if defined(DEBUG_BUILD)
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

    return true;
}

void RenderFrame()
{
    d3dContext->OMSetRenderTargets(1, &backBufferView, nullptr);

    float backgroundColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
    d3dContext->ClearRenderTargetView(backBufferView, backgroundColor);

    d3dContext->IASetInputLayout(inputLayout);
    d3dContext->VSSetShader(vertexShader, nullptr, 0);
    d3dContext->PSSetShader(pixelShader, nullptr, 0);

    dxgiSwapchain->Present(0, 0);
}

int main()
{
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);

    appDirectory = std::filesystem::path(std::string(exePath)).parent_path();

    if (!InitWindow()) return -1;
    if (!InitD3D()) return -1;

    while (true)
    {
        UpdateWindow();
        RenderFrame();
    }

    return 0;
}
