#include "RenderingSystem.h"
#include <algorithm>
#include <iostream>
#include "..//Engine/Entities/EntityFactory.h"
#include "../Engine/CoreInput/CoreInput.h"
#include "../Engine/GEngine.h"
#include "../Engine/MathLibrary/MathLibrary.h"
#include "../FileIO/FileIO.h"
#include "PostProcess/Bloom.h"

#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")

#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#define NOMINMAX
#include <Windows.h>
#include <windowsx.h>

#include <wrl/client.h>
#include "../Utility/Macros/DirectXMacros.h"

#include "../Engine/ResourceManager/AnimationClip.h"
#include "../Engine/ResourceManager/GeometryShader.h"
#include "../Engine/ResourceManager/Material.h"
#include "../Engine/ResourceManager/PixelShader.h"
#include "../Engine/ResourceManager/SkeletalMesh.h"
#include "../Engine/ResourceManager/StaticMesh.h"
#include "../Engine/ResourceManager/Texture2D.h"
#include "../Engine/ResourceManager/VertexShader.h"

// Will refactor this
#include "../Engine/Gameplay/OrbitSystem.h"
#include "../Engine/Gameplay/SpeedboostSystem.h"

#include "../Engine/GenericComponents/TransformComponent.h"
#include "Vertex.h"

#include "../Engine/Particle Systems/ParticleManager.h"
#include "../UI/UIManager.h"

#include "Components/CameraComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Terrain/TerrainManager.h"

#include "..//Engine/Controller/ControllerSystem.h"
#include "../Engine/MathLibrary/ColorConstants.h"
#include "../Utility/MemoryLeakDetection.h"
#include "DebugRender/debug_renderer.h"

void RenderSystem::CreateDeviceAndSwapChain()
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
        sd.BufferCount        = 3;
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
        hr                 = m_Device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debug));
        assert(SUCCEEDED(hr));
        // debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        debug->Release();
#endif

        device->Release();
        context->Release();
}

void RenderSystem::CreateDefaultRenderTargets(D3D11_TEXTURE2D_DESC* backbufferDesc)
{
        HRESULT          hr;
        ID3D11Texture2D* pBackBuffer;

        // Release all outstanding references to the swap chain's buffers.
        for (int i = 0; i < E_RENDER_TARGET::COUNT; ++i)
        {
                SAFE_RELEASE(m_DefaultRenderTargets[i]);
        }

        for (int i = 0; i < E_DEPTH_STENCIL::COUNT; ++i)
        {
                SAFE_RELEASE(m_DefaultDepthStencil[i]);
        }

        for (int i = 0; i < E_POSTPROCESS_PIXEL_SRV::COUNT; ++i)
        {
                SAFE_RELEASE(m_PostProcessSRVs[i]);
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
        if (backbufferDesc)
                *backbufferDesc = textureDesc;
        pBackBuffer->Release();

        m_BackBufferWidth  = (float)textureDesc.Width;
        m_BackBufferHeight = (float)textureDesc.Height;

        ID3D11Texture2D* texture;

        hr = m_Device->CreateTexture2D(&textureDesc, NULL, &texture);
        assert(SUCCEEDED(hr));
        hr = m_Device->CreateRenderTargetView(texture, NULL, &m_DefaultRenderTargets[E_RENDER_TARGET::BASE_PASS]);
        assert(SUCCEEDED(hr));
        hr = m_Device->CreateShaderResourceView(texture, nullptr, &m_PostProcessSRVs[E_POSTPROCESS_PIXEL_SRV::BASE_PASS]);
        assert(SUCCEEDED(hr));
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

        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc{};
        viewDesc.Format              = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        viewDesc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
        viewDesc.Texture2D.MipLevels = 1;
        m_Device->CreateShaderResourceView(texture, &viewDesc, &m_PostProcessSRVs[E_POSTPROCESS_PIXEL_SRV::BASE_DEPTH]);
        texture->Release();

        assert(SUCCEEDED(hr));
}

void RenderSystem::CreateRasterizerStates()
{
        CD3D11_RASTERIZER_DESC desc(D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE, 0, 0.f, 0.f, TRUE, FALSE, FALSE, FALSE);
        m_Device->CreateRasterizerState(&desc, &m_DefaultRasterizerStates[E_RASTERIZER_STATE::DEFAULT]);

        desc.AntialiasedLineEnable = false;
        desc.CullMode              = D3D11_CULL_NONE;
        m_Device->CreateRasterizerState(&desc, &m_DefaultRasterizerStates[E_RASTERIZER_STATE::LINE]);

        desc.FillMode              = D3D11_FILL_WIREFRAME;
        desc.AntialiasedLineEnable = false;
        m_Device->CreateRasterizerState(&desc, &m_DefaultRasterizerStates[E_RASTERIZER_STATE::WIREFRAME]);
}

void RenderSystem::CreateInputLayouts()
{
        D3D11_INPUT_ELEMENT_DESC vLayout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        FileIO::FShaderData shaderData;
        EResult             er = FileIO::LoadShaderDataFromFile("Default", "_VS", &shaderData);

        assert(er.m_Flags == ERESULT_FLAG::SUCCESS);

        HRESULT hr = m_Device->CreateInputLayout(vLayout,
                                                 ARRAYSIZE(vLayout),
                                                 shaderData.bytes.data(),
                                                 shaderData.bytes.size(),
                                                 &m_DefaultInputLayouts[E_INPUT_LAYOUT::DEFAULT]);
        assert(SUCCEEDED(hr));


        D3D11_INPUT_ELEMENT_DESC vLayoutSkinned[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"JOINTINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        er = FileIO::LoadShaderDataFromFile("Default_Skinned", "_VS", &shaderData);

        assert(er.m_Flags == ERESULT_FLAG::SUCCESS);

        hr = m_Device->CreateInputLayout(vLayoutSkinned,
                                         ARRAYSIZE(vLayoutSkinned),
                                         shaderData.bytes.data(),
                                         shaderData.bytes.size(),
                                         &m_DefaultInputLayouts[E_INPUT_LAYOUT::SKINNED]);
        assert(SUCCEEDED(hr));

        D3D11_INPUT_ELEMENT_DESC vLayoutDebug[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        er = FileIO::LoadShaderDataFromFile("Debug", "_VS", &shaderData);

        assert(er.m_Flags == ERESULT_FLAG::SUCCESS);

        hr = m_Device->CreateInputLayout(vLayoutDebug,
                                         ARRAYSIZE(vLayoutDebug),
                                         shaderData.bytes.data(),
                                         shaderData.bytes.size(),
                                         &m_DefaultInputLayouts[E_INPUT_LAYOUT::DEBUG]);

        assert(SUCCEEDED(hr));

        D3D11_INPUT_ELEMENT_DESC vLayoutLine[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        er = FileIO::LoadShaderDataFromFile("Line", "_VS", &shaderData);

        assert(er.m_Flags == ERESULT_FLAG::SUCCESS);

        hr = m_Device->CreateInputLayout(vLayoutLine,
                                         ARRAYSIZE(vLayoutLine),
                                         shaderData.bytes.data(),
                                         shaderData.bytes.size(),
                                         &m_DefaultInputLayouts[E_INPUT_LAYOUT::LINE]);
}

void RenderSystem::CreateCommonShaders()
{
        m_CommonVertexShaderHandles[E_VERTEX_SHADERS::DEFAULT] = m_ResourceManager->LoadVertexShader("Default");
        m_CommonVertexShaderHandles[E_VERTEX_SHADERS::SKINNED] = m_ResourceManager->LoadVertexShader("Default_Skinned");
        m_CommonVertexShaderHandles[E_VERTEX_SHADERS::DEBUG]   = m_ResourceManager->LoadVertexShader("Debug");
        m_CommonVertexShaderHandles[E_VERTEX_SHADERS::LINE]    = m_ResourceManager->LoadVertexShader("Line");
        m_CommonPixelShaderHandles[E_PIXEL_SHADERS::DEFAULT]   = m_ResourceManager->LoadPixelShader("Default");
        m_CommonPixelShaderHandles[E_PIXEL_SHADERS::DEBUG]     = m_ResourceManager->LoadPixelShader("Debug");
        m_CommonPixelShaderHandles[E_PIXEL_SHADERS::LINE]      = m_ResourceManager->LoadPixelShader("Line");
        m_LineGeometryShader                                   = m_ResourceManager->LoadGeometryShader("Line");
}

void RenderSystem::CreateCommonConstantBuffers()
{
        HRESULT           hr{};
        D3D11_BUFFER_DESC bd{};
        bd.Usage          = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth      = sizeof(CTransformBuffer);
        bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        hr |= m_Device->CreateBuffer(&bd, nullptr, &m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::MVP]);

        bd.ByteWidth = sizeof(CSceneInfoBuffer);
        hr |= m_Device->CreateBuffer(&bd, nullptr, &m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::SCENE]);

        bd.ByteWidth = sizeof(FSurfaceProperties);
        hr |= m_Device->CreateBuffer(&bd, nullptr, &m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::SURFACE]);

        bd.ByteWidth = sizeof(CAnimationBuffer);
        hr |= m_Device->CreateBuffer(&bd, nullptr, &m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::ANIM]);

        bd.ByteWidth = sizeof(CScreenSpaceBuffer);
        hr |= m_Device->CreateBuffer(&bd, nullptr, &m_PostProcessConstantBuffers[E_CONSTANT_BUFFER_POST_PROCESS::SCREENSPACE]);

        assert(SUCCEEDED(hr));
}

void RenderSystem::CreateSamplerStates()
{
        HRESULT hr;

        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter             = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU           = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV           = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW           = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.MipLODBias         = 0.0f;
        sampDesc.MaxAnisotropy      = 1;
        sampDesc.ComparisonFunc     = D3D11_COMPARISON_ALWAYS;
        sampDesc.BorderColor[0]     = 0;
        sampDesc.BorderColor[1]     = 0;
        sampDesc.BorderColor[2]     = 0;
        sampDesc.BorderColor[3]     = 0;
        sampDesc.MinLOD             = 0;
        sampDesc.MaxLOD             = D3D11_FLOAT32_MAX;

        hr = m_Device->CreateSamplerState(&sampDesc, &m_DefaultSamplerStates[E_SAMPLER_STATE::WRAP]);
        assert(SUCCEEDED(hr));

        sampDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.MipLODBias     = 0.0f;
        sampDesc.MaxAnisotropy  = 1;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampDesc.BorderColor[0] = 0;
        sampDesc.BorderColor[1] = 0;
        sampDesc.BorderColor[2] = 0;
        sampDesc.BorderColor[3] = 0;
        sampDesc.MinLOD         = 0;
        sampDesc.MaxLOD         = D3D11_FLOAT32_MAX;

        hr = m_Device->CreateSamplerState(&sampDesc, &m_DefaultSamplerStates[E_SAMPLER_STATE::CLAMP]);
        assert(SUCCEEDED(hr));


        sampDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

        hr = m_Device->CreateSamplerState(&sampDesc, &m_DefaultSamplerStates[E_SAMPLER_STATE::SKY]);
        assert(SUCCEEDED(hr));


        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
        sampDesc.Filter   = D3D11_FILTER_MIN_MAG_MIP_POINT;
        hr                = m_Device->CreateSamplerState(&sampDesc, &m_DefaultSamplerStates[E_SAMPLER_STATE::NEAREST]);
        assert(SUCCEEDED(hr));


        // Create the sample state clamp
        float               black[] = {0.f, 0.f, 0.f, 1.f};
        CD3D11_SAMPLER_DESC SamDescShad(D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
                                        D3D11_TEXTURE_ADDRESS_BORDER,
                                        D3D11_TEXTURE_ADDRESS_BORDER,
                                        D3D11_TEXTURE_ADDRESS_BORDER,
                                        0.f,
                                        0,
                                        D3D11_COMPARISON_LESS,
                                        black,
                                        0,
                                        0);
        hr = m_Device->CreateSamplerState(&SamDescShad, &m_DefaultSamplerStates[E_SAMPLER_STATE::SHADOWS]);
        assert(SUCCEEDED(hr));
}

void RenderSystem::CreateBlendStates()
{
        CD3D11_BLEND_DESC blendDescOpaque{D3D11_DEFAULT};
        m_Device->CreateBlendState(&blendDescOpaque, &m_BlendStates[E_BLEND_STATE::Opaque]);

        CD3D11_BLEND_DESC blendDescTransluscent{};

        blendDescTransluscent.RenderTarget[0].BlendEnable           = true;
        blendDescTransluscent.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
        blendDescTransluscent.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
        blendDescTransluscent.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
        blendDescTransluscent.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_INV_DEST_ALPHA;
        blendDescTransluscent.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
        blendDescTransluscent.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
        blendDescTransluscent.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        m_Device->CreateBlendState(&blendDescTransluscent, &m_BlendStates[E_BLEND_STATE::Transluscent]);

        CD3D11_BLEND_DESC blendAdditive{};

        blendAdditive.RenderTarget[0].BlendEnable           = true;
        blendAdditive.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
        blendAdditive.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
        blendAdditive.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
        blendAdditive.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
        blendAdditive.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
        blendAdditive.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
        blendAdditive.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        m_Device->CreateBlendState(&blendDescTransluscent, &m_BlendStates[E_BLEND_STATE::Additive]);
}

void RenderSystem::CreateDepthStencilStates()
{
        CD3D11_DEPTH_STENCIL_DESC desc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
        desc.DepthFunc                 = D3D11_COMPARISON_LESS_EQUAL;
        desc.DepthEnable               = true;
        m_Device->CreateDepthStencilState(&desc, &m_DepthStencilStates[E_DEPTH_STENCIL_STATE::BASE_PASS]);
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        m_Device->CreateDepthStencilState(&desc, &m_DepthStencilStates[E_DEPTH_STENCIL_STATE::TRANSLUSCENT]);
}

void RenderSystem::CreatePostProcessEffects(D3D11_TEXTURE2D_DESC* desc)
{
        for (auto& pp : m_PostProcessChain)
        {
                pp->Shutdown();
                delete pp;
        }
        m_PostProcessChain.clear();

        m_PostProcessChain.push_back(DBG_NEW Bloom);
        m_PostProcessChain.front()->Initialize(m_Device, m_Context, desc);
}

void RenderSystem::CreateDebugBuffers()
{
        D3D11_BUFFER_DESC desc{};
        desc.Usage          = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth      = UINT(sizeof(FDebugVertex) * debug_renderer::get_line_vert_capacity());
        desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT hr = m_Device->CreateBuffer(&desc, nullptr, &m_DebugVertexBuffer);

        desc.ByteWidth = UINT(sizeof(DirectX::XMVECTOR) * 2000);
        hr |= m_Device->CreateBuffer(&desc, nullptr, &m_LineVertexBuffer);

        assert(SUCCEEDED(hr));
}


void RenderSystem::DrawDebug()
{
        UINT stride = sizeof(FDebugVertex);
        UINT offset = 0;

        // m_Context->RSSetState(m_DefaultRasterizerStates[E_RASTERIZER_STATE::D]);

        D3D11_MAPPED_SUBRESOURCE mappedResource{};
        m_Context->Map(m_DebugVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy(mappedResource.pData,
               debug_renderer::get_line_verts(),
               debug_renderer::get_line_vert_count() * sizeof(FDebugVertex));
        m_Context->Unmap(m_DebugVertexBuffer, 0);

        m_Context->IASetVertexBuffers(0, 1, &m_DebugVertexBuffer, &stride, &offset);
        m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        m_Context->IASetInputLayout(m_DefaultInputLayouts[E_INPUT_LAYOUT::DEBUG]);
        ID3D11VertexShader* vs =
            m_ResourceManager->GetResource<VertexShader>(m_CommonVertexShaderHandles[E_VERTEX_SHADERS::DEBUG])->m_VertexShader;
        ID3D11PixelShader* ps =
            m_ResourceManager->GetResource<PixelShader>(m_CommonVertexShaderHandles[E_PIXEL_SHADERS::DEBUG])->m_PixelShader;

        m_Context->VSSetShader(vs, 0, 0);
        m_Context->VSSetConstantBuffers(
            E_CONSTANT_BUFFER_BASE_PASS::MVP, 1, &m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::MVP]);
        m_Context->PSSetShader(ps, 0, 0);
        m_Context->Draw((UINT)debug_renderer::get_line_vert_count(), 0);
}

void RenderSystem::DrawLines()
{
        // return;
        using namespace DirectX;

        UINT stride = sizeof(SplinePoint);
        UINT offset = 0;

        m_Context->RSSetState(m_DefaultRasterizerStates[E_RASTERIZER_STATE::LINE]);
        if (GCoreInput::GetKeyState(KeyCode::Shift) == KeyState::Down)
                m_Context->RSSetState(m_DefaultRasterizerStates[E_RASTERIZER_STATE::WIREFRAME]);

        SpeedBoostSystem* speedboostSystem = GEngine::Get()->GetSystemManager()->GetSystem<SpeedBoostSystem>();
        OrbitSystem*      orbitSystem      = GEngine::Get()->GetSystemManager()->GetSystem<OrbitSystem>();

        m_Context->IASetVertexBuffers(0, 1, &m_LineVertexBuffer, &stride, &offset);
        m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
        m_Context->IASetInputLayout(m_DefaultInputLayouts[E_INPUT_LAYOUT::LINE]);
        ID3D11VertexShader* vs =
            m_ResourceManager->GetResource<VertexShader>(m_CommonVertexShaderHandles[E_VERTEX_SHADERS::LINE])->m_VertexShader;
        ID3D11PixelShader* ps =
            m_ResourceManager->GetResource<PixelShader>(m_CommonVertexShaderHandles[E_PIXEL_SHADERS::LINE])->m_PixelShader;
        ID3D11GeometryShader* gs = m_ResourceManager->GetResource<GeometryShader>(m_LineGeometryShader)->m_GeometryShader;

        m_Context->VSSetShader(vs, 0, 0);
        m_Context->VSSetConstantBuffers(
            E_CONSTANT_BUFFER_BASE_PASS::MVP, 1, &m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::MVP]);
        m_Context->PSSetShader(ps, 0, 0);
        m_Context->GSSetShader(gs, 0, 0);
        m_Context->GSSetConstantBuffers(
            E_CONSTANT_BUFFER_BASE_PASS::MVP, 1, &m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::MVP]);
        m_Context->GSSetConstantBuffers(
            E_CONSTANT_BUFFER_BASE_PASS::SCENE, 1, &m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::SCENE]);
        FSurfaceProperties surf;

        for (auto& it : speedboostSystem->m_SplineClusterSpawners)
        {
                int vertexCount = it.second.cachedPoints.size() - 1;

                int separator  = it.second.deleteIndex;
                int separation = it.second.deleteSeparation;

                int lengthA = separator - separation;

                int startB  = separator + separation;
                int lengthB = vertexCount - startB + 1;


                XMStoreFloat3(&surf.emissiveColor,
                              2.0f * DirectX::PackedVector::XMLoadColor(&E_LIGHT_ORBS::ORB_COLORS[it.second.targetColor]));

                UpdateConstantBuffer(
                    m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::SURFACE], &surf, sizeof(FSurfaceProperties));


                if (lengthA > 1)
                {
                        D3D11_MAPPED_SUBRESOURCE mappedResource{};
                        m_Context->Map(m_LineVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

                        memcpy(mappedResource.pData, it.second.cachedPoints.data(), (lengthA) * sizeof(SplinePoint));
                        m_Context->Unmap(m_LineVertexBuffer, 0);

                        m_Context->Draw(lengthA, 0);
                }

                if (lengthB > 1)
                {
                        D3D11_MAPPED_SUBRESOURCE mappedResource{};
                        m_Context->Map(m_LineVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
                        memcpy(mappedResource.pData, it.second.cachedPoints.data() + startB, (lengthB) * sizeof(SplinePoint));
                        m_Context->Unmap(m_LineVertexBuffer, 0);

                        m_Context->Draw(lengthB, 0);
                }
        }
        ID3D11GeometryShader* nullGS = nullptr;
        m_Context->GSSetShader(nullGS, 0, 0);


        /*if (orbitSystem->activeGoal.hasActiveGoal == true)
        {
                int vertexCount = 2;

                XMVECTOR vec[2];
                vec[0] = orbitSystem->activeGoal.activeGoalGround.GetComponent<TransformComponent>()->transform.translation;
                vec[1] = orbitSystem->activeGoal.activeGoalOrbit.GetComponent<TransformComponent>()->transform.translation;

                D3D11_MAPPED_SUBRESOURCE mappedResource{};
                m_Context->Map(m_LineVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
                memcpy(mappedResource.pData, vec, vertexCount * sizeof(XMVECTOR));
                m_Context->Unmap(m_LineVertexBuffer, 0);

                XMStoreFloat3(&surf.emissiveColor,
                              1.2f * DirectX::PackedVector::XMLoadColor(&E_LIGHT_ORBS::ORB_COLORS[E_LIGHT_ORBS::WHITE_LIGHTS]));

                UpdateConstantBuffer(
                    m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::SURFACE], &surf, sizeof(FSurfaceProperties));

                m_Context->Draw(vertexCount, 0);
        }*/

        m_Context->RSSetState(m_DefaultRasterizerStates[E_RASTERIZER_STATE::DEFAULT]);
        m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_Context->IASetInputLayout(m_DefaultInputLayouts[E_INPUT_LAYOUT::DEFAULT]);
}

void RenderSystem::UpdateConstantBuffer(ID3D11Buffer* gpuBuffer, void* cpuBuffer, size_t size)
{
        D3D11_MAPPED_SUBRESOURCE mappedResource{};
        m_Context->Map(gpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy(mappedResource.pData, cpuBuffer, size);
        m_Context->Unmap(gpuBuffer, 0);
}

void RenderSystem::DrawMesh(ID3D11Buffer*      vertexBuffer,
                            ID3D11Buffer*      indexBuffer,
                            uint32_t           indexCount,
                            uint32_t           vertexSize,
                            Material*          material,
                            DirectX::XMMATRIX* mtx)
{
        using namespace DirectX;

        const UINT strides[] = {vertexSize};
        const UINT offsets[] = {0};

        m_Context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
        m_Context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

        VertexShader* vs = m_ResourceManager->GetResource<VertexShader>(material->m_VertexShaderHandle);
        PixelShader*  ps = m_ResourceManager->GetResource<PixelShader>(material->m_PixelShaderHandle);

        m_Context->VSSetShader(vs->m_VertexShader, nullptr, 0);
        m_Context->PSSetShader(ps->m_PixelShader, nullptr, 0);

        ID3D11ShaderResourceView* srvs[E_BASE_PASS_PIXEL_SRV::PER_MAT_COUNT];
        m_ResourceManager->GetSRVsFromMaterial(material, srvs);

        m_Context->PSSetShaderResources(0, E_BASE_PASS_PIXEL_SRV::PER_MAT_COUNT, srvs);
        m_Context->VSSetShaderResources(0, E_BASE_PASS_PIXEL_SRV::PER_MAT_COUNT, srvs);

        m_ConstantBuffer_MVP.World = XMMatrixTranspose(*mtx);

        UpdateConstantBuffer(
            m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::MVP], &m_ConstantBuffer_MVP, sizeof(m_ConstantBuffer_MVP));
        UpdateConstantBuffer(m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::SURFACE],
                             &material->m_SurfaceProperties,
                             sizeof(FSurfaceProperties));

        m_Context->DrawIndexed(indexCount, 0, 0);
}

void RenderSystem::DrawMeshInstanced(ID3D11Buffer* vertexBuffer,
                                     ID3D11Buffer* indexBuffer,
                                     uint32_t      indexCount,
                                     uint32_t      vertexSize,
                                     Material*     material,
                                     uint32_t      instanceCount,
                                     uint32_t      instanceOffset)
{
        using namespace DirectX;

        const UINT strides[] = {vertexSize};
        const UINT offsets[] = {0};

        m_Context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
        m_Context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        VertexShader* vs = m_ResourceManager->GetResource<VertexShader>(material->m_VertexShaderHandle);
        PixelShader*  ps = m_ResourceManager->GetResource<PixelShader>(material->m_PixelShaderHandle);

        m_Context->VSSetShader(vs->m_VertexShader, nullptr, 0);
        m_Context->PSSetShader(ps->m_PixelShader, nullptr, 0);

        ID3D11ShaderResourceView* srvs[E_BASE_PASS_PIXEL_SRV::PER_MAT_COUNT];
        m_ResourceManager->GetSRVsFromMaterial(material, srvs);

        m_Context->PSSetShaderResources(0, E_BASE_PASS_PIXEL_SRV::PER_MAT_COUNT, srvs);

        UpdateConstantBuffer(m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::SURFACE],
                             &material->m_SurfaceProperties,
                             sizeof(FSurfaceProperties));

        m_Context->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, instanceOffset);
}

void RenderSystem::DrawStaticMesh(StaticMesh* mesh, Material* material, DirectX::XMMATRIX* mtx)
{
        using namespace DirectX;
        m_Context->IASetInputLayout(m_DefaultInputLayouts[E_INPUT_LAYOUT::DEFAULT]);
        DrawMesh(mesh->m_VertexBuffer, mesh->m_IndexBuffer, mesh->m_IndexCount, sizeof(FVertex), material, mtx);
}

void RenderSystem::DrawSkeletalMesh(SkeletalMesh*               mesh,
                                    Material*                   material,
                                    DirectX::XMMATRIX*          mtx,
                                    const Animation::FSkeleton* skel)
{
        using namespace DirectX;

        const UINT strides[] = {sizeof(FSkinnedVertex)};
        const UINT offsets[] = {0};

        int jointCount = (int)skel->jointTransforms.size();


        for (int i = 0; i < jointCount; ++i)
        {
                m_ConstantBuffer_ANIM.jointTransforms[i] = skel->jointTransforms[i].transform.CreateMatrix();
        }

        for (int i = 1; i < jointCount; ++i)
        {
                XMMATRIX parent = m_ConstantBuffer_ANIM.jointTransforms[skel->jointTransforms[i].parent_index];
                m_ConstantBuffer_ANIM.jointTransforms[i] = m_ConstantBuffer_ANIM.jointTransforms[i] * parent;
        }
        /** If Debug? Debug renderer**/
        debug_renderer::AddBoneHierarchy(*skel, m_ConstantBuffer_ANIM.jointTransforms, *mtx, ColorConstants::White, 0.2f);

        for (int i = 0; i < jointCount; ++i)
        {
                m_ConstantBuffer_ANIM.jointTransforms[i] = XMMatrixTranspose(skel->inverseBindPose[i].transform.CreateMatrix() *
                                                                             m_ConstantBuffer_ANIM.jointTransforms[i]);
        }

        UpdateConstantBuffer(m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::ANIM],
                             &m_ConstantBuffer_ANIM,
                             sizeof(m_ConstantBuffer_ANIM));
        m_Context->IASetInputLayout(m_DefaultInputLayouts[E_INPUT_LAYOUT::SKINNED]);

        DrawMesh(mesh->m_VertexBuffer, mesh->m_IndexBuffer, mesh->m_IndexCount, sizeof(FSkinnedVertex), material, mtx);
}

void RenderSystem::RefreshMainCameraSettings()
{
        using namespace DirectX;

        CameraComponent* camera = m_MainCameraHandle.Get<CameraComponent>();
        camera->dirty           = false;
        auto& settings          = camera->m_Settings;
        settings.m_AspectRatio  = GetWindowAspectRatio();
        float verticalFOV       = XMConvertToRadians(settings.m_HorizontalFOV / settings.m_AspectRatio);

        m_CachedMainProjectionMatrix =
            DirectX::XMMatrixPerspectiveFovLH(verticalFOV, settings.m_AspectRatio, settings.m_NearClip, settings.m_FarClip);
        m_CachedMainInvProjectionMatrix = XMMatrixInverse(nullptr, m_CachedMainProjectionMatrix);
}

void RenderSystem::OnPreUpdate(float deltaTime)
{
        using namespace std;
        using namespace DirectX;

        ID3D11ShaderResourceView* srvs[5] = {0};
        srvs[0] = m_ResourceManager->GetResource<Texture2D>(m_ResourceManager->LoadTexture2D("IBLTestDiffuseHDR"))->m_SRV;
        srvs[1] = m_ResourceManager->GetResource<Texture2D>(m_ResourceManager->LoadTexture2D("IBLTestSpecularHDR"))->m_SRV;
        srvs[2] = m_ResourceManager->GetResource<Texture2D>(m_ResourceManager->LoadTexture2D("IBLTestBrdf"))->m_SRV;
        srvs[3] =
            m_ResourceManager->GetResource<Texture2D>(m_ResourceManager->LoadTexture2D("Clouds_Fibers_BlurredNoise"))->m_SRV;
        srvs[4] = m_ResourceManager->GetResource<Texture2D>(m_ResourceManager->LoadTexture2D("Veins_Caustics_Tiles"))->m_SRV;

        m_Context->PSSetShaderResources(E_BASE_PASS_PIXEL_SRV::PER_MAT_COUNT, 5, srvs);
        m_Context->VSSetShaderResources(E_BASE_PASS_PIXEL_SRV::PER_MAT_COUNT + 3, 2, &srvs[3]);

        /*********/
        ControllerSystem* controllerSystem = GEngine::Get()->GetSystemManager()->GetSystem<ControllerSystem>();

        m_ContstantBuffer_SCREENSPACE.selectionColor = controllerSystem->GetCurrentColorSelection();
        m_ContstantBuffer_SCREENSPACE.selectionAlpha = controllerSystem->GetCurrentColorAlpha();

        /*********/

        /** Update Camera Info **/
        CameraComponent* mainCamera = m_MainCameraHandle.Get<CameraComponent>();

        if (mainCamera->dirty)
                RefreshMainCameraSettings();

        EntityHandle        mainCameraEntity = mainCamera->GetParent();
        TransformComponent* mainTransform    = mainCameraEntity.GetComponent<TransformComponent>();

        m_CachedMainInvViewMatrix        = mainTransform->transform.CreateMatrix();
        XMMATRIX view                    = XMMatrixInverse(nullptr, m_CachedMainInvViewMatrix);
        m_CachedMainViewMatrix           = view;
        m_CachedMainViewProjectionMatrix = view * m_CachedMainProjectionMatrix;

        mainCamera->_cachedView           = view;
        mainCamera->_cachedProjection     = m_CachedMainProjectionMatrix;
        mainCamera->_cachedViewProjection = m_CachedMainViewProjectionMatrix;

        m_SkyHandle.Get<TransformComponent>()->transform.translation = mainTransform->transform.translation;

        XMStoreFloat3(&m_ConstantBuffer_SCENE.eyePosition, mainTransform->transform.translation);
        m_ConstantBuffer_MVP.ViewProjection = XMMatrixTranspose(m_CachedMainViewProjectionMatrix);
        m_ConstantBuffer_MVP.Projection     = XMMatrixTranspose(m_CachedMainProjectionMatrix);
        // get scale
        m_ConstantBuffer_SCENE.scale            = TerrainManager::Get()->GetScale();
        m_ConstantBuffer_SCENE.screenDimensions = XMFLOAT2(m_BackBufferWidth, m_BackBufferHeight);
        XMStoreFloat3(&m_ConstantBuffer_SCENE.worldOffsetDelta, GEngine::Get()->m_WorldOffsetDelta);

        m_ConstantBuffer_SCENE._InstanceReveal = GEngine::Get()->m_InstanceReveal;
        m_ConstantBuffer_SCENE.puzzleState     = GEngine::Get()->m_CurrentPuzzleState;

        /** Prepare draw calls **/
        m_TransluscentDraws.clear();
        m_OpaqueDraws.clear();
        // Add all static meshes


        for (auto& staticMeshComp : m_HandleManager->GetActiveComponents<StaticMeshComponent>())
        {
                FDraw drawcall;

                drawcall.meshType        = FDraw::EDrawType::Static;
                drawcall.componentHandle = staticMeshComp.GetHandle();

                StaticMesh*         staticMesh = m_ResourceManager->GetResource<StaticMesh>(staticMeshComp.m_StaticMeshHandle);
                EntityHandle        entityHandle = staticMeshComp.GetParent();
                TransformComponent* tcomp        = entityHandle.GetComponent<TransformComponent>();
                Material*           mat          = m_ResourceManager->GetResource<Material>(staticMeshComp.m_MaterialHandle);

                drawcall.meshResource   = staticMeshComp.m_StaticMeshHandle;
                drawcall.materialHandle = staticMeshComp.m_MaterialHandle;
                drawcall.mtx            = tcomp->transform.CreateMatrix();

                if (mat->m_SurfaceProperties.textureFlags & SURFACE_FLAG_IS_TRANSLUSCENT)
                {
                        m_TransluscentDraws.emplace_back(std::move(drawcall));
                }
                else
                {
                        m_OpaqueDraws.emplace_back(std::move(drawcall));
                }
        }


        // Add all skeletal meshes


        for (auto& skelMeshComp : m_HandleManager->GetActiveComponents<SkeletalMeshComponent>())
        {
                FDraw drawcall;
                drawcall.meshType        = FDraw::EDrawType::Skeletal;
                drawcall.componentHandle = skelMeshComp.GetHandle();

                SkeletalMesh*       skelMesh = m_ResourceManager->GetResource<SkeletalMesh>(skelMeshComp.m_SkeletalMeshHandle);
                EntityHandle        entityHandle = skelMeshComp.GetParent();
                TransformComponent* tcomp        = entityHandle.GetComponent<TransformComponent>();
                Material*           mat          = m_ResourceManager->GetResource<Material>(skelMeshComp.m_MaterialHandle);

                drawcall.meshResource   = skelMeshComp.m_SkeletalMeshHandle;
                drawcall.materialHandle = skelMeshComp.m_MaterialHandle;
                drawcall.mtx            = tcomp->transform.CreateMatrix();

                if (mat->m_SurfaceProperties.textureFlags & SURFACE_FLAG_IS_TRANSLUSCENT)
                {
                        m_TransluscentDraws.emplace_back(std::move(drawcall));
                }
                else
                {
                        m_OpaqueDraws.emplace_back(std::move(drawcall));
                }
        }

        // Sort opaque meshes front to back
        std::sort(m_OpaqueDraws.begin(), m_OpaqueDraws.end(), [this](const FDraw& a, const FDraw& b) -> bool {
                XMVECTOR va = XMVector3TransformCoord(a.mtx.r[3], m_CachedMainViewProjectionMatrix);
                XMVECTOR vb = XMVector3TransformCoord(b.mtx.r[3], m_CachedMainViewProjectionMatrix);

                return XMVectorGetZ(va) < XMVectorGetZ(vb);
        });

        // Sort transluscent meshes back to front
        std::sort(m_OpaqueDraws.begin(), m_OpaqueDraws.end(), [this](const FDraw& a, const FDraw& b) -> bool {
                XMVECTOR va = XMVector3TransformCoord(a.mtx.r[3], m_CachedMainViewProjectionMatrix);
                XMVECTOR vb = XMVector3TransformCoord(b.mtx.r[3], m_CachedMainViewProjectionMatrix);

                return XMVectorGetZ(va) > XMVectorGetZ(vb);
        });
}

void RenderSystem::OnUpdate(float deltaTime)
{
        using namespace DirectX;

        ResourceManager* rm = GEngine::Get()->GetResourceManager();

        m_Context->VSSetSamplers(0, E_SAMPLER_STATE::COUNT, m_DefaultSamplerStates);
        m_Context->PSSetSamplers(0, E_SAMPLER_STATE::COUNT, m_DefaultSamplerStates);
        m_Context->HSSetSamplers(0, E_SAMPLER_STATE::COUNT, m_DefaultSamplerStates);
        m_Context->DSSetSamplers(0, E_SAMPLER_STATE::COUNT, m_DefaultSamplerStates);
        m_Context->CSSetSamplers(0, E_SAMPLER_STATE::COUNT, m_DefaultSamplerStates);

        // Set base pass texture as render target
        m_Context->OMSetRenderTargets(
            1, &m_DefaultRenderTargets[E_RENDER_TARGET::BASE_PASS], m_DefaultDepthStencil[E_DEPTH_STENCIL::BASE_PASS]);

        /** Test clear render target **/
        static FLOAT color[] = {0.0f, 0.0f, 0.0f, 0.0f};
        m_Context->ClearRenderTargetView(m_DefaultRenderTargets[E_RENDER_TARGET::BASE_PASS], color);
        m_Context->ClearDepthStencilView(
            m_DefaultDepthStencil[E_DEPTH_STENCIL::BASE_PASS], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        /** Create viewport. This should be replaced**/
        D3D11_VIEWPORT viewport;
        viewport.Height   = m_BackBufferHeight;
        viewport.Width    = m_BackBufferWidth;
        viewport.MaxDepth = 1.0f;
        viewport.MinDepth = 0.0f;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;


        m_Context->RSSetViewports(1, &viewport);

        m_Context->VSSetConstantBuffers(0, E_CONSTANT_BUFFER_BASE_PASS::COUNT, m_BasePassConstantBuffers);
        m_Context->PSSetConstantBuffers(0, E_CONSTANT_BUFFER_BASE_PASS::COUNT, m_BasePassConstantBuffers);
        m_Context->HSSetConstantBuffers(0, 2, m_BasePassConstantBuffers);
        m_Context->DSSetConstantBuffers(0, 2, m_BasePassConstantBuffers);

        /** Get Directional Light**/
        {

                for (auto dirLightComp : m_HandleManager->GetActiveComponents<DirectionalLightComponent>())
                {

                        XMStoreFloat3(&m_ConstantBuffer_SCENE.directionalLightDirection,
                                      dirLightComp.m_LightRotation.GetForward());

                        m_ConstantBuffer_SCENE.directionalLightColor =
                            XMFLOAT3A(dirLightComp.m_LightColor.x * dirLightComp.m_LightColor.w,
                                      dirLightComp.m_LightColor.y * dirLightComp.m_LightColor.w,
                                      dirLightComp.m_LightColor.z * dirLightComp.m_LightColor.w);

                        m_ConstantBuffer_SCENE.ambientColor =
                            XMFLOAT3A(dirLightComp.m_AmbientColor.x * dirLightComp.m_AmbientColor.w,
                                      dirLightComp.m_AmbientColor.y * dirLightComp.m_AmbientColor.w,
                                      dirLightComp.m_AmbientColor.z * dirLightComp.m_AmbientColor.w);
                }
        }
        m_ConstantBuffer_SCENE.aspectRatio  = m_BackBufferWidth / m_BackBufferHeight;
        m_ConstantBuffer_SCENE.time         = (float)GEngine::Get()->GetTotalTime();
        m_ConstantBuffer_SCENE.deltaTime    = deltaTime;
        m_ConstantBuffer_SCENE.playerRadius = (float)GEngine::Get()->GetCurrentPlayerRadius();
        UpdateConstantBuffer(m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::SCENE],
                             &m_ConstantBuffer_SCENE,
                             sizeof(m_ConstantBuffer_SCENE));

        float blendFactor[] = {0.75f, 0.75f, 0.75f, 1.0f};
        UINT  sampleMask    = 0xffffffff;

        m_Context->RSSetState(m_DefaultRasterizerStates[E_RASTERIZER_STATE::DEFAULT]);
        if (GCoreInput::GetKeyState(KeyCode::Shift) == KeyState::Down)
                m_Context->RSSetState(m_DefaultRasterizerStates[E_RASTERIZER_STATE::WIREFRAME]);
        /*** Render Terrain Start ***/
        m_Context->OMSetDepthStencilState(m_DepthStencilStates[E_DEPTH_STENCIL_STATE::BASE_PASS], 0);
        m_Context->OMSetBlendState(m_BlendStates[E_BLEND_STATE::Opaque], 0, sampleMask);
        TerrainManager::Update(deltaTime);
        /*** Render Terrain End ***/

        m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        /** Render opaque meshes **/
        for (size_t i = 0, n = m_OpaqueDraws.size(); i < n; ++i)
        {
                /*if (m_OpaqueDraws[i].meshType == FDraw::EDrawType::Static)
                {*/
                StaticMesh* mesh = m_ResourceManager->GetResource<StaticMesh>(m_OpaqueDraws[i].meshResource);
                Material*   mat  = m_ResourceManager->GetResource<Material>(m_OpaqueDraws[i].materialHandle);
                DrawStaticMesh(mesh, mat, &m_OpaqueDraws[i].mtx);
                /*}
                else
                {
                        SkeletalMesh* mesh = m_ResourceManager->GetResource<SkeletalMesh>(m_OpaqueDraws[i].meshResource);
                        Material*     mat  = m_ResourceManager->GetResource<Material>(m_OpaqueDraws[i].materialHandle);
                        SkeletalMeshComponent* meshComp = m_OpaqueDraws[i].componentHandle.Get<SkeletalMeshComponent>();
                        DrawSkeletalMesh(mesh, mat, &m_OpaqueDraws[i].mtx, &meshComp->m_Skeleton);
                }*/
        }


        /** Render transluscent meshes **/
        m_Context->OMSetDepthStencilState(m_DepthStencilStates[E_DEPTH_STENCIL_STATE::TRANSLUSCENT], 1);
        m_Context->OMSetBlendState(m_BlendStates[E_BLEND_STATE::Additive], blendFactor, sampleMask);
        for (size_t i = 0, n = m_TransluscentDraws.size(); i < n; ++i)
        {
                /*if (m_TransluscentDraws[i].meshType == FDraw::EDrawType::Static)
                {*/
                StaticMesh* mesh = m_ResourceManager->GetResource<StaticMesh>(m_TransluscentDraws[i].meshResource);
                Material*   mat  = m_ResourceManager->GetResource<Material>(m_TransluscentDraws[i].materialHandle);
                DrawStaticMesh(mesh, mat, &m_TransluscentDraws[i].mtx);
                /*}
                else
                {
                        SkeletalMesh* mesh = m_ResourceManager->GetResource<SkeletalMesh>(m_TransluscentDraws[i].meshResource);
                        Material*     mat  = m_ResourceManager->GetResource<Material>(m_TransluscentDraws[i].materialHandle);
                        SkeletalMeshComponent* meshComp = m_TransluscentDraws[i].componentHandle.Get<SkeletalMeshComponent>();
                        DrawSkeletalMesh(mesh, mat, &m_TransluscentDraws[i].mtx, &meshComp->m_Skeleton);
                }*/
        }
        DrawLines();

        m_Context->RSSetState(m_DefaultRasterizerStates[E_RASTERIZER_STATE::DEFAULT]);

        ParticleManager::Update(deltaTime);

        // Set the backbuffer as render target
        m_Context->OMSetRenderTargets(1, &m_DefaultRenderTargets[E_RENDER_TARGET::BACKBUFFER], nullptr);

        /** Move from base pass to backbuffer. Should be replaced by post processing **/
        /*ID3D11Resource* dest;
        ID3D11Resource* src;
        m_DefaultRenderTargets[E_RENDER_TARGET::BACKBUFFER]->GetResource(&dest);
        m_DefaultRenderTargets[E_RENDER_TARGET::BASE_PASS]->GetResource(&src);
        m_Context->CopyResource(dest, src);
        dest->Release();
        src->Release();*/
        m_Context->PSSetShaderResources(0, E_POSTPROCESS_PIXEL_SRV::COUNT, m_PostProcessSRVs);
        m_Context->PSSetConstantBuffers(0, E_CONSTANT_BUFFER_POST_PROCESS::COUNT, m_PostProcessConstantBuffers);
        m_ContstantBuffer_SCREENSPACE.invProj        = XMMatrixTranspose(m_CachedMainInvProjectionMatrix);
        m_ContstantBuffer_SCREENSPACE.invView        = XMMatrixTranspose(m_CachedMainInvViewMatrix);
        m_ContstantBuffer_SCREENSPACE.Proj           = m_ConstantBuffer_MVP.Projection;
        m_ContstantBuffer_SCREENSPACE.time           = m_ConstantBuffer_SCENE.time;
        m_ContstantBuffer_SCREENSPACE.playerPosition = m_ConstantBuffer_SCENE.eyePosition;

        UpdateConstantBuffer(m_PostProcessConstantBuffers[E_CONSTANT_BUFFER_POST_PROCESS::SCREENSPACE],
                             &m_ContstantBuffer_SCREENSPACE,
                             sizeof(m_ContstantBuffer_SCREENSPACE));

        m_Context->OMSetBlendState(m_BlendStates[E_BLEND_STATE::Opaque], blendFactor, sampleMask);

        ID3D11ShaderResourceView* inSRV  = m_PostProcessSRVs[E_POSTPROCESS_PIXEL_SRV::BASE_PASS];
        ID3D11RenderTargetView*   outRTV = m_DefaultRenderTargets[E_RENDER_TARGET::BACKBUFFER];
        for (auto& pp : m_PostProcessChain)
        {
                pp->Render(&inSRV, &outRTV);
        }

        m_Context->OMSetBlendState(m_BlendStates[E_BLEND_STATE::Opaque], blendFactor, sampleMask);
        if (GEngine::Get()->IsDebugMode())
                DrawDebug();
        debug_renderer::clear_lines();

        // UI Manager Update
        UIManager::Update();

        DXGI_PRESENT_PARAMETERS parameters = {0};
        m_Swapchain->Present1(0, 0, &parameters);
}

void RenderSystem::OnPostUpdate(float deltaTime)
{
        ID3D11ShaderResourceView* srvs[7] = {};
        m_Context->VSSetShaderResources(0, 7, srvs);
        m_Context->PSSetShaderResources(0, 7, srvs);
        m_Context->OMSetRenderTargets(0, nullptr, nullptr);
}

void RenderSystem::OnInitialize()
{
        assert(m_WindowHandle);

        m_ResourceManager = GEngine::Get()->GetResourceManager();
        m_HandleManager   = GEngine::Get()->GetHandleManager();

        CreateDeviceAndSwapChain();
        D3D11_TEXTURE2D_DESC desc;
        CreateDefaultRenderTargets(&desc);
        CreateRasterizerStates();
        CreateCommonShaders();
        CreateInputLayouts();
        CreateCommonConstantBuffers();
        CreateSamplerStates();
        CreateDebugBuffers();
        CreatePostProcessEffects(&desc);
        CreateBlendStates();
        CreateDepthStencilStates();

        // UI Manager Initialize
        UIManager::Initialize(m_WindowHandle);
        TerrainManager::Initialize(this);
        ParticleManager::Initialize();
        {
                EntityFactory::CreateStaticMeshEntity("SkyDome01", "SkyMat01", &m_SkyHandle);
                auto transComp = m_SkyHandle.Get<TransformComponent>();
                transComp->transform.SetScale(1400.0f);
        }
}

void RenderSystem::OnShutdown()
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

        for (int i = 0; i < E_INPUT_LAYOUT::COUNT; ++i)
        {

                SAFE_RELEASE(m_DefaultInputLayouts[i]);
        }

        for (int i = 0; i < E_RASTERIZER_STATE::COUNT; ++i)
        {

                SAFE_RELEASE(m_DefaultRasterizerStates[i]);
        }

        for (int i = 0; i < E_CONSTANT_BUFFER_BASE_PASS::COUNT; ++i)
        {

                SAFE_RELEASE(m_BasePassConstantBuffers[i]);
        }

        for (int i = 0; i < E_SAMPLER_STATE::COUNT; ++i)
        {

                SAFE_RELEASE(m_DefaultSamplerStates[i]);
        }

        for (int i = 0; i < E_BLEND_STATE::COUNT; ++i)
        {

                SAFE_RELEASE(m_BlendStates[i]);
        }

        for (int i = 0; i < E_DEPTH_STENCIL_STATE::COUNT; ++i)
        {

                SAFE_RELEASE(m_DepthStencilStates[i]);
        }

        SAFE_RELEASE(m_DebugVertexBuffer);
        SAFE_RELEASE(m_LineVertexBuffer);

        SAFE_RELEASE(m_Swapchain);
        SAFE_RELEASE(m_Context);
        SAFE_RELEASE(m_Device);

        for (int i = 0; i < E_CONSTANT_BUFFER_POST_PROCESS::COUNT; ++i)
        {

                SAFE_RELEASE(m_PostProcessConstantBuffers[i]);
        }

        for (auto& pp : m_PostProcessChain)
        {
                pp->Shutdown();
                delete pp;
        }
        // UI Manager Shutdown
        UIManager::Shutdown();

        TerrainManager::Shutdown();

        ParticleManager::Shutdown();
}

void RenderSystem::OnResume()
{}

void RenderSystem::OnSuspend()
{}

void RenderSystem::SetWindowHandle(native_handle_type handle)
{
        m_WindowHandle = handle;
}

void RenderSystem::SetMainCameraComponent(ComponentHandle cameraHandle)
{
        m_MainCameraHandle = cameraHandle;

        RefreshMainCameraSettings();
}

void RenderSystem::OnWindowResize(WPARAM wParam, LPARAM lParam)
{
        if (m_Swapchain)
        {
                D3D11_TEXTURE2D_DESC desc;
                CreateDefaultRenderTargets(&desc);
                CreatePostProcessEffects(&desc);
                RefreshMainCameraSettings();

                std::cout << m_BackBufferHeight << "   " << m_BackBufferWidth << std::endl;
        }
}

void RenderSystem::SetFullscreen(bool val)
{
        if (val)
        {
                DXGI_MODE_DESC desc{};
                desc.Format  = DXGI_FORMAT_B8G8R8A8_UNORM;
                desc.Height  = m_BackBufferHeight;
                desc.Width   = m_BackBufferHeight;
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

bool RenderSystem::GetFullscreen()
{
        BOOL val;
        m_Swapchain->GetFullscreenState(&val, nullptr);
        return val;
}
