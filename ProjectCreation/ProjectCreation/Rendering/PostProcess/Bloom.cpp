#include "Bloom.h"

#include <assert.h>
#include <d3d11_1.h>
#include <math.h>

#include "../../Engine/GEngine.h"
#include "../../Engine/ResourceManager/PixelShader.h"
#include "../../Engine/ResourceManager/Texture2D.h"
#include "../../Engine/ResourceManager/VertexShader.h"

#include "../../Utility/Macros/DirectXMacros.h"
#include "../../Utility/RenderUtility.h"

#include "../..//Engine/MathLibrary/ColorConstants.h"
#include "../..//Engine/MathLibrary/MathLibrary.h"

#include "../RenderingSystem.h"

void Bloom::Initialize(ID3D11Device1* device, ID3D11DeviceContext1* context, D3D11_TEXTURE2D_DESC* _desc)
{
        HRESULT hr;

        m_Device  = device;
        m_Context = context;

        D3D11_TEXTURE2D_DESC desc = *_desc;

        m_Width  = desc.Width;
        m_Height = desc.Height;

        m_ResourceManager = GEngine::Get()->GetResourceManager();

        m_BloomCB_CPU.inverseScreenDimensions.x = 1.0f / m_Width;
        m_BloomCB_CPU.inverseScreenDimensions.y = 1.0f / m_Height;

        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

        {
                hr = m_Device->CreateTexture2D(&desc, nullptr, &m_BlurTextures[Bloom::E_PASSES::MASK]);
                assert(SUCCEEDED(hr));

                D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
                rtvDesc.Format             = desc.Format;
                rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
                rtvDesc.Texture2D.MipSlice = 0;

                hr = m_Device->CreateRenderTargetView(
                    m_BlurTextures[Bloom::E_PASSES::MASK], &rtvDesc, &m_BlurRTVs[Bloom::E_PASSES::MASK]);
                assert(SUCCEEDED(hr));

                m_Device->CreateShaderResourceView(
                    m_BlurTextures[Bloom::E_PASSES::MASK], nullptr, &m_BlurSRVs[Bloom::E_PASSES::MASK]);
                assert(SUCCEEDED(hr));
        }

        { // AO
                auto newDesc   = desc;
                newDesc.Format = DXGI_FORMAT_R8_UNORM;
                hr             = m_Device->CreateTexture2D(&newDesc, nullptr, &m_BlurTextures[Bloom::E_PASSES::AO]);
                assert(SUCCEEDED(hr));

                D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
                rtvDesc.Format             = newDesc.Format;
                rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
                rtvDesc.Texture2D.MipSlice = 0;

                hr = m_Device->CreateRenderTargetView(
                    m_BlurTextures[Bloom::E_PASSES::AO], &rtvDesc, &m_BlurRTVs[Bloom::E_PASSES::AO]);
                assert(SUCCEEDED(hr));

                m_Device->CreateShaderResourceView(
                    m_BlurTextures[Bloom::E_PASSES::AO], nullptr, &m_BlurSRVs[Bloom::E_PASSES::AO]);
                assert(SUCCEEDED(hr));
        }

        for (int i = 0; i < MaxIterations; ++i)
        {
                UINT div    = (UINT)pow(2, i);
                desc.Width  = UINT(m_Width / div) > 0 ? UINT(m_Width / div) : 1;
                desc.Height = UINT(m_Height / div) > 0 ? UINT(m_Height / div) : 1;
                hr          = m_Device->CreateTexture2D(&desc, nullptr, &m_BlurTextures[i]);
                assert(SUCCEEDED(hr));

                D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
                rtvDesc.Format             = desc.Format;
                rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
                rtvDesc.Texture2D.MipSlice = 0;

                hr = m_Device->CreateRenderTargetView(m_BlurTextures[i], &rtvDesc, &m_BlurRTVs[i]);
                assert(SUCCEEDED(hr));

                m_Device->CreateShaderResourceView(m_BlurTextures[i], nullptr, &m_BlurSRVs[i]);
                assert(SUCCEEDED(hr));
        }

        m_ScreenQuadVS          = m_ResourceManager->LoadVertexShader("ScreenQuad");
        m_BloomMaskPS           = m_ResourceManager->LoadPixelShader("BloomMask");
        m_BloomCombinePS        = m_ResourceManager->LoadPixelShader("BloomCombine");
        m_BloomUpscalePS        = m_ResourceManager->LoadPixelShader("BloomUpscale");
        m_BloomDownscalePS      = m_ResourceManager->LoadPixelShader("BloomDownscale");
        m_BloomDownscaleKarisPS = m_ResourceManager->LoadPixelShader("BloomDownscaleKaris");
        m_AO_PS                 = m_ResourceManager->LoadPixelShader("AO");
        m_AOBlur_PS             = m_ResourceManager->LoadPixelShader("AOBlur");
        m_AONormals             = m_ResourceManager->LoadTexture2D("RandomNormalsAO");

        D3D11_BUFFER_DESC cbDesc{};
        cbDesc.Usage          = D3D11_USAGE_DYNAMIC;
        cbDesc.ByteWidth      = sizeof(BloomCB);
        cbDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        hr                    = m_Device->CreateBuffer(&cbDesc, nullptr, &m_BloomCB_GPU);
        assert(SUCCEEDED(hr));

        CD3D11_BLEND_DESC blendDesc{D3D11_DEFAULT};

        m_Device->CreateBlendState(&blendDesc, &m_DefaultBlendState);

        blendDesc.AlphaToCoverageEnable          = false;
        blendDesc.IndependentBlendEnable         = false;
        blendDesc.RenderTarget[0].BlendEnable    = true;
        blendDesc.RenderTarget[0].SrcBlend       = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend      = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;

        m_Device->CreateBlendState(&blendDesc, &m_BlendState);
}

void Bloom::Render(ID3D11ShaderResourceView** inSRV, ID3D11RenderTargetView** outRTV)
{
        D3D11_VIEWPORT viewport;
        viewport.MaxDepth = 1.0f;
        viewport.MinDepth = 0.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;

        VertexShader* screenQuadVS     = m_ResourceManager->GetResource<VertexShader>(m_ScreenQuadVS);
        PixelShader*  maskPS           = m_ResourceManager->GetResource<PixelShader>(m_BloomMaskPS);
        PixelShader*  combinePS        = m_ResourceManager->GetResource<PixelShader>(m_BloomCombinePS);
        PixelShader*  upscalePS        = m_ResourceManager->GetResource<PixelShader>(m_BloomUpscalePS);
        PixelShader*  downscalePS      = m_ResourceManager->GetResource<PixelShader>(m_BloomDownscalePS);
        PixelShader*  downscaleKarisPS = m_ResourceManager->GetResource<PixelShader>(m_BloomDownscaleKarisPS);
        PixelShader*  AO_PS            = m_ResourceManager->GetResource<PixelShader>(m_AO_PS);
        PixelShader*  AOBlur_PS        = m_ResourceManager->GetResource<PixelShader>(m_AOBlur_PS);


        int   s           = std::max(m_Width / 2, m_Height / 2);
        float logs        = log2f((float)s) + std::min(9.0f, 10.0f) - 10.0f;
        float logs_i      = floorf(logs);
        int   iterations  = (int)MathLibrary::clamp(logs_i, 1.0f, (float)MaxIterations);
        float sampleScale = 0.5f + logs - logs_i;

        m_Context->IASetInputLayout(nullptr);
        m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        m_Context->VSSetShader(screenQuadVS->m_VertexShader, nullptr, 0);
        m_Context->PSSetConstantBuffers(10, 1, &m_BloomCB_GPU);

        m_Context->OMSetRenderTargets(1, &m_BlurRTVs[Bloom::E_PASSES::MASK], nullptr);
        m_Context->PSSetShaderResources(0, 1, inSRV);
        m_Context->PSSetShader(maskPS->m_PixelShader, nullptr, 0);
        m_Context->Draw(4, 0);

        m_Context->PSSetShader(downscaleKarisPS->m_PixelShader, nullptr, 0);
        m_Context->OMSetRenderTargets(0, nullptr, nullptr);
        m_Context->PSSetShaderResources(0, 1, &m_BlurSRVs[Bloom::E_PASSES::MASK]);

        {
                UINT div        = (UINT)pow(2, 0);
                viewport.Width  = FLOAT(m_Width / div);
                viewport.Height = FLOAT(m_Height / div);

                m_Context->RSSetViewports(1, &viewport);
                // m_Context->ClearRenderTargetView(m_BlurRTVs[i], black);
                m_Context->OMSetRenderTargets(1, &m_BlurRTVs[0], nullptr);
                m_Context->Draw(4, 0);
                m_Context->OMSetRenderTargets(0, nullptr, nullptr);
                m_Context->PSSetShaderResources(E_BLOOM_PS_SRV::BLOOM, 1, &m_BlurSRVs[0]);
        }

        m_Context->PSSetShader(downscalePS->m_PixelShader, nullptr, 0);

        for (int i = 1; i < iterations; ++i)
        {
                UINT div        = (UINT)pow(2, i);
                viewport.Width  = FLOAT(m_Width / div);
                viewport.Height = FLOAT(m_Height / div);

                m_BloomCB_CPU.inverseScreenDimensions = DirectX::XMFLOAT2(1 / viewport.Width, 1 / viewport.Height);
                RenderUtility::UpdateConstantBuffer(m_Context, m_BloomCB_GPU, &m_BloomCB_CPU, sizeof(m_BloomCB_CPU));
                m_Context->RSSetViewports(1, &viewport);
                // m_Context->ClearRenderTargetView(m_BlurRTVs[i], black);
                m_Context->OMSetRenderTargets(1, &m_BlurRTVs[i], nullptr);
                m_Context->Draw(4, 0);
                m_Context->OMSetRenderTargets(0, nullptr, nullptr);
                m_Context->PSSetShaderResources(E_BLOOM_PS_SRV::BLOOM, 1, &m_BlurSRVs[i]);
        }

        // ID3D11ShaderResourceView* nullsrv[8] = {};
        // m_Context->PSSetShaderResources(E_BLOOM_PS_SRV::BLOOM, 1, nullsrv);
        m_Context->PSSetShader(upscalePS->m_PixelShader, nullptr, 0);

        float blendFactor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        UINT  sampleMask     = 0xffffffff;
        m_Context->OMSetBlendState(m_BlendState, blendFactor, sampleMask);

        float _storedBrightness  = m_BloomCB_CPU.brightness;
        m_BloomCB_CPU.blurRadius = sampleScale;
        m_BloomCB_CPU.hOrV       = 1;
        for (int i = iterations - 2; i >= 0; --i)
        {
                UINT div                              = (UINT)pow(2, i);
                viewport.Width                        = FLOAT(m_Width / div);
                viewport.Height                       = FLOAT(m_Height / div);
                m_BloomCB_CPU.inverseScreenDimensions = DirectX::XMFLOAT2(1 / viewport.Width, 1 / viewport.Height);

                RenderUtility::UpdateConstantBuffer(m_Context, m_BloomCB_GPU, &m_BloomCB_CPU, sizeof(m_BloomCB_CPU));


                m_Context->RSSetViewports(1, &viewport);
                m_Context->OMSetRenderTargets(1, &m_BlurRTVs[i], nullptr);
                m_Context->Draw(4, 0);
                m_Context->OMSetRenderTargets(0, nullptr, nullptr);
                m_Context->PSSetShaderResources(E_BLOOM_PS_SRV::BLOOM, 1, &m_BlurSRVs[i]);
        }
        m_BloomCB_CPU.brightness = _storedBrightness;

        RenderUtility::UpdateConstantBuffer(m_Context, m_BloomCB_GPU, &m_BloomCB_CPU, sizeof(m_BloomCB_CPU));
        m_Context->OMSetBlendState(m_DefaultBlendState, blendFactor, sampleMask);



        Texture2D* aoTex = m_ResourceManager->GetResource<Texture2D>(m_AONormals);
        m_Context->PSSetShaderResources(3, 1, &aoTex->m_SRV);

        // Do Ambient occlusion pass
        constexpr FLOAT black[4] = {0.0f, 0.0f, 0.0f, 0.0f};

        //m_Context->ClearRenderTargetView(m_BlurRTVs[E_PASSES::AO], black);
        m_Context->PSSetShader(AO_PS->m_PixelShader, nullptr, 0);
        ID3D11ShaderResourceView* nullSRV = nullptr;
        m_Context->PSSetShaderResources(4, 1, &nullSRV);
        m_Context->OMSetRenderTargets(1, &m_BlurRTVs[E_PASSES::AO], nullptr);
        m_Context->Draw(4, 0);

        // Blur AO
        UINT AOTargetIndex = E_PASSES::MASK;
        m_Context->PSSetShader(AOBlur_PS->m_PixelShader, nullptr, 0);
		
        {
                UINT div        = (UINT)pow(2, 0);
                viewport.Width  = FLOAT(m_Width / div);
                viewport.Height = FLOAT(m_Height / div);

                m_BloomCB_CPU.inverseScreenDimensions = DirectX::XMFLOAT2(1 / viewport.Width, 1 / viewport.Height);
                RenderUtility::UpdateConstantBuffer(m_Context, m_BloomCB_GPU, &m_BloomCB_CPU, sizeof(m_BloomCB_CPU));
                m_Context->RSSetViewports(1, &viewport);
                //m_Context->ClearRenderTargetView(m_BlurRTVs[AOTargetIndex], black);
                m_Context->PSSetShaderResources(E_BLOOM_PS_SRV::BLOOM, 1, &nullSRV);
                m_Context->OMSetRenderTargets(1, &m_BlurRTVs[AOTargetIndex], nullptr);
                m_Context->PSSetShaderResources(E_BLOOM_PS_SRV::BLOOM, 1, &m_BlurSRVs[E_PASSES::AO]);
                m_Context->Draw(4, 0);
        }

        UINT div        = (UINT)pow(2, 0);
        viewport.Width  = FLOAT(m_Width / div);
        viewport.Height = FLOAT(m_Height / div);
        m_Context->RSSetViewports(1, &viewport);

        // do bloom combine stage
        m_Context->PSSetShader(combinePS->m_PixelShader, nullptr, 0);
        m_Context->OMSetRenderTargets(1, outRTV, nullptr);
        m_Context->PSSetShaderResources(4, 1, &m_BlurSRVs[AOTargetIndex]);
        m_Context->PSSetShaderResources(E_BLOOM_PS_SRV::BLOOM, 1, &m_BlurSRVs[0]);
        m_Context->PSSetShaderResources(E_BLOOM_PS_SRV::SCREEN, 1, inSRV);
        m_Context->Draw(4, 0);
}

void Bloom::Shutdown()
{
        for (int i = 0; i < TotalPasses; ++i)
        {
                SAFE_RELEASE(m_BlurRTVs[i]);
                SAFE_RELEASE(m_BlurTextures[i]);
                SAFE_RELEASE(m_BlurSRVs[i]);
        }

        SAFE_RELEASE(m_DefaultBlendState);
        SAFE_RELEASE(m_BlendState);
        SAFE_RELEASE(m_BloomCB_GPU);
}
