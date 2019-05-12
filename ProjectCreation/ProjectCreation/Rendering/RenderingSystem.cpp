#include "RenderingSystem.h"
#include "../CoreInput/CoreInput.h"

#include <math.h>

#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")

#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#include <Windows.h>
#include <windowsx.h>


#include <wrl/client.h>


void CRenderSystem::CreateDeviceAndSwapChain()
{
        using namespace Microsoft::WRL;

        ID3D11Device*        device;
        ID3D11DeviceContext* context;
        HRESULT              hr;

#ifdef _DEBUG
        UINT flags = D3D11_CREATE_DEVICE_DEBUG;
#else
        UINT flags = 0;
#endif
        flags = 0;

        D3D_FEATURE_LEVEL FeatureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        UINT              numFeatureLevels = ARRAYSIZE(FeatureLevels);
        D3D_FEATURE_LEVEL FeatureLevelObtained;


        hr = D3D11CreateDevice(nullptr,
                               D3D_DRIVER_TYPE_HARDWARE,
                               nullptr,
                               flags,
                               FeatureLevels,
                               numFeatureLevels,
                               D3D11_SDK_VERSION,
                               &device,
                               &FeatureLevelObtained,
                               &context);

        assert(SUCCEEDED(hr));
        // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
        hr = D3D11CreateDevice(nullptr,
                               D3D_DRIVER_TYPE_HARDWARE,
                               nullptr,
                               flags,
                               FeatureLevels,
                               numFeatureLevels,
                               D3D11_SDK_VERSION,
                               &device,
                               &FeatureLevelObtained,
                               &context);
        assert(SUCCEEDED(hr));

        hr = device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&m_Device));

        assert(SUCCEEDED(hr));

        hr = context->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&m_Context));
        assert(SUCCEEDED(hr));

        // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
        IDXGIFactory1* dxgiFactory = nullptr;
        {
                IDXGIDevice* dxgiDevice = nullptr;
                hr = m_Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
                if (SUCCEEDED(hr))
                {
                        IDXGIAdapter* adapter;
                        hr = dxgiDevice->GetAdapter(&adapter);
                        if (SUCCEEDED(hr))
                        {
                                hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
                        }
                }
        }

        assert(SUCCEEDED(hr));
        // Create swap chain
        IDXGIFactory2* dxgiFactory2 = nullptr;
        hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));

        assert(SUCCEEDED(hr));

        RECT rect;
        bool bWindowRect = GetWindowRect((HWND)m_WindowHandle, &rect);
        assert(bWindowRect);

        DXGI_SWAP_CHAIN_DESC1 sd{};
        sd.Width              = UINT(rect.right - rect.left);
        sd.Height             = UINT(rect.bottom - rect.top);
        sd.Format             = DXGI_FORMAT_B8G8R8A8_UNORM;
        sd.Stereo             = false;
        sd.SampleDesc.Count   = 1; // Don't use multi-sampling.
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount        = 2;
        sd.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        sd.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //| DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fd{};
        fd.RefreshRate.Numerator   = 0;
        fd.RefreshRate.Denominator = 0;
        fd.Windowed                = true;
        fd.Scaling                 = DXGI_MODE_SCALING_STRETCHED;

        hr = dxgiFactory2->CreateSwapChainForHwnd(m_Device, (HWND)m_WindowHandle, &sd, &fd, nullptr, &m_Swapchain);
        // dxgiFactory->MakeWindowAssociation(window->GetHandle(), DXGI_MWA_NO_ALT_ENTER);
        dxgiFactory2->Release();
}

void CRenderSystem::CreateBackbufferRenderTarget()
{
        HRESULT          hr;
        ID3D11Texture2D* pBackBuffer;

        // Release all outstanding references to the swap chain's buffers.
        if (m_RenderTargets[ERENDER_TARGET::BACKBUFFER])
                m_RenderTargets[ERENDER_TARGET::BACKBUFFER]->Release();
        m_Context->OMSetRenderTargets(0, 0, 0);

        // Preserve the existing buffer count and format.
        // Automatically choose the width and height to match the client rect for HWNDs.
        hr = m_Swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

        // Perform error handling here!
        assert(SUCCEEDED(hr));

        hr = m_Swapchain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        assert(SUCCEEDED(hr));
        hr = m_Device->CreateRenderTargetView(pBackBuffer, NULL, &m_RenderTargets[ERENDER_TARGET::BACKBUFFER]);
        assert(SUCCEEDED(hr));
        pBackBuffer->Release();
}

void CRenderSystem::CreateRasterizerStates()
{}

void CRenderSystem::OnPreUpdate(float deltaTime)
{
        if (GCoreInput::GetKeyState(KeyCode::W) == KeyState::DownFirst)
        {
                SetFullscreen(!GetFullscreen());
        }
}

void CRenderSystem::OnUpdate(float deltaTime)
{
        m_Context->OMSetRenderTargets(1, &m_RenderTargets[ERENDER_TARGET::BACKBUFFER], nullptr);
        static FLOAT color[]   = {1.0f, 1.0f, 1.0f, 1.0f};
        static float totalTime = 0.0f;
        totalTime += deltaTime;
        color[0] = sin(totalTime);
        color[1] = cos(totalTime);
        m_Context->ClearRenderTargetView(m_RenderTargets[ERENDER_TARGET::BACKBUFFER], color);
}

void CRenderSystem::OnPostUpdate(float deltaTime)
{
        DXGI_PRESENT_PARAMETERS parameters = {0};
        m_Swapchain->Present1(1, 0, &parameters);
}

void CRenderSystem::OnInitialize()
{
        assert(m_WindowHandle);

        CreateDeviceAndSwapChain();
        CreateBackbufferRenderTarget();
        CreateRasterizerStates();
}

void CRenderSystem::OnShutdown()
{
        SetFullscreen(false);

        m_Swapchain->Release();
        m_Context->Release();
        m_Device->Release();
}

void CRenderSystem::OnResume()
{}

void CRenderSystem::OnSuspend()
{}

void CRenderSystem::SetWindowHandle(native_handle_type handle)
{
        m_WindowHandle = handle;
}

void CRenderSystem::OnWindowResize(WPARAM wParam, LPARAM lParam)
{
        if (m_Swapchain)
        {
                CreateBackbufferRenderTarget();
        }
}

void CRenderSystem::SetFullscreen(bool val)
{
        if (val)
        {
                DXGI_MODE_DESC desc{};
                desc.Format  = DXGI_FORMAT_B8G8R8A8_UNORM;
                desc.Height  = 1920;
                desc.Width   = 1080;
                desc.Scaling = DXGI_MODE_SCALING_STRETCHED;

                IDXGIOutput* target = nullptr;
                m_Swapchain->SetFullscreenState(true, target);
                m_Swapchain->ResizeTarget(&desc);
        }
        else
        {
                m_Swapchain->SetFullscreenState(false, nullptr);
        }
}

bool CRenderSystem::GetFullscreen()
{
        BOOL val;
        m_Swapchain->GetFullscreenState(&val, nullptr);
        return val;
}
