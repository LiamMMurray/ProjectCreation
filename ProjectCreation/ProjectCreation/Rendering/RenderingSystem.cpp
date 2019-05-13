#include "RenderingSystem.h"
#include "../CoreInput/CoreInput.h"

#include <math.h>

#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")

#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#include <Windows.h>
#include <windowsx.h>


#include <wrl/client.h>
#include "../Utility/Macros/DirectXMacros.h"

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
        // flags = 0;

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

        // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
        if (FAILED(hr))
        {
                hr = D3D11CreateDevice(nullptr,
                                       D3D_DRIVER_TYPE_HARDWARE,
                                       nullptr,
                                       flags,
                                       &FeatureLevels[1],
                                       numFeatureLevels - 1,
                                       D3D11_SDK_VERSION,
                                       &device,
                                       &FeatureLevelObtained,
                                       &context);
        }
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
                                adapter->Release();
                        }
                        dxgiDevice->Release();
                }
        }

        assert(SUCCEEDED(hr));
        // Create swap chain
        IDXGIFactory2* dxgiFactory2 = nullptr;
        hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
        dxgiFactory->Release();

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

		#ifdef _DEBUG

        ID3D11Debug* debug = nullptr;
        hr = m_Device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debug));
        assert(SUCCEEDED(hr));
        //debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        debug->Release();
		#endif

        device->Release();
        context->Release();
}

void CRenderSystem::CreateDefaultRenderTargets()
{
        HRESULT          hr;
        ID3D11Texture2D* pBackBuffer;

        // Release all outstanding references to the swap chain's buffers.
        for (int i = 0; i < E_RENDER_TARGET::COUNT; ++i)
        {
                SAFE_RELEASE(m_DefaultRenderTargets[i]);
        }


        m_Context->OMSetRenderTargets(0, 0, 0);

        // Preserve the existing buffer count and format.
        // Automatically choose the width and height to match the client rect for HWNDs.
        hr = m_Swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

        // Perform error handling here!
        assert(SUCCEEDED(hr));

        // Create backbuffer render target
        hr = m_Swapchain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        assert(SUCCEEDED(hr));
        hr = m_Device->CreateRenderTargetView(pBackBuffer, NULL, &m_DefaultRenderTargets[E_RENDER_TARGET::BACKBUFFER]);

        // Create Base Pass render target and resource view
        D3D11_TEXTURE2D_DESC textureDesc;
        pBackBuffer->GetDesc(&textureDesc);
        textureDesc.Format    = DXGI_FORMAT_R16G16B16A16_FLOAT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        pBackBuffer->Release();

        ID3D11Texture2D* texture;

        hr = m_Device->CreateTexture2D(&textureDesc, NULL, &texture);
        assert(SUCCEEDED(hr));
        hr = m_Device->CreateRenderTargetView(texture, NULL, &m_DefaultRenderTargets[E_RENDER_TARGET::BASE_PASS]);
        texture->Release();

        // Create Base Pass depth stencil and resource view

        // Create depth stencil texture
        D3D11_TEXTURE2D_DESC descDepth{};
        descDepth.Width              = textureDesc.Width;
        descDepth.Height             = textureDesc.Height;
        descDepth.MipLevels          = 1;
        descDepth.ArraySize          = 1;
        descDepth.Format             = DXGI_FORMAT_R24G8_TYPELESS;
        descDepth.SampleDesc.Count   = 1;
        descDepth.SampleDesc.Quality = 0;
        descDepth.Usage              = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        descDepth.CPUAccessFlags     = 0;
        descDepth.MiscFlags          = 0;
        hr                           = m_Device->CreateTexture2D(&descDepth, nullptr, &texture);
        assert(SUCCEEDED(hr));

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV{};
        descDSV.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
        descDSV.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
        hr = m_Device->CreateDepthStencilView(texture, &descDSV, &m_DefaultDepthStencil[E_DEPTH_STENCIL::BASE_PASS]);
        texture->Release();

        assert(SUCCEEDED(hr));
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
        // Cache transforms. Update dirty transforms

        // Sort scene front to back

        // Perform culling
}

void CRenderSystem::OnPostUpdate(float deltaTime)
{
        m_Context->OMSetRenderTargets(
            1, &m_DefaultRenderTargets[E_RENDER_TARGET::BASE_PASS], m_DefaultDepthStencil[E_DEPTH_STENCIL::BASE_PASS]);
        static FLOAT color[]   = {1.0f, 1.0f, 1.0f, 1.0f};
        static float totalTime = 0.0f;
        totalTime += deltaTime;
        color[0] = sin(totalTime);
        color[1] = cos(totalTime);

        m_Context->ClearRenderTargetView(m_DefaultRenderTargets[E_RENDER_TARGET::BASE_PASS], color);
        m_Context->ClearDepthStencilView(
            m_DefaultDepthStencil[E_DEPTH_STENCIL::BASE_PASS], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        m_Context->OMSetRenderTargets(1, &m_DefaultRenderTargets[E_RENDER_TARGET::BACKBUFFER], nullptr);

        DXGI_PRESENT_PARAMETERS parameters = {0};
        m_Swapchain->Present1(1, 0, &parameters);
}

void CRenderSystem::OnInitialize()
{
        assert(m_WindowHandle);

        CreateDeviceAndSwapChain();
        CreateDefaultRenderTargets();
        CreateRasterizerStates();
}

void CRenderSystem::OnShutdown()
{
        SetFullscreen(false);

        for (int i = 0; i < E_RENDER_TARGET::COUNT; ++i)
        {
                SAFE_RELEASE(m_DefaultRenderTargets[i]);
        }

		for (int i = 0; i < E_POSTPROCESS_PIXEL_SRV::COUNT; ++i)
        {

                SAFE_RELEASE(m_PostProcessSRVs[i]);
        }

		for (int i = 0; i < E_DEPTH_STENCIL::COUNT; ++i)
        {

                SAFE_RELEASE(m_DefaultDepthStencil[i]);
        }

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
                CreateDefaultRenderTargets();
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
