#pragma once
#include "PostProcessEffectBase.h"

#include <DirectXMath.h>
#include <stdint.h>

#include <IResource.h>

struct E_BLOOM_PS_SRV
{
        enum
        {
                BLOOM  = 0,
                DEPTH  = 1,
                SCREEN = 2,
                COUNT
        };
};


struct alignas(16) BloomCB
{
        float             threshold  = 1.0f;
        float             brightness = 1.0f;
        float             blurRadius;
        unsigned int      hOrV; // horizontal or vertical
        DirectX::XMFLOAT2 inverseScreenDimensions;
};

class ResourceManager;

class Bloom : public PostProcessEffectBase
{
        static constexpr uint8_t MaxIterations = 16;

        struct E_PASSES
        {
                enum
                {
                        MASK = Bloom::MaxIterations,
                        AO,
                        COUNT
                };
        };
        static constexpr uint8_t TotalPasses = E_PASSES::COUNT;

        BloomCB       m_BloomCB_CPU;
        ID3D11Buffer* m_BloomCB_GPU;

        ID3D11BlendState* m_BlendState;
        ID3D11BlendState* m_DefaultBlendState;

        ID3D11RenderTargetView*   m_BlurRTVs[TotalPasses];
        ID3D11Texture2D*          m_BlurTextures[TotalPasses];
        ID3D11ShaderResourceView* m_BlurSRVs[TotalPasses];

        uint32_t m_Width;
        uint32_t m_Height;

        ResourceManager* m_ResourceManager;

        ResourceHandle m_ScreenQuadVS;
        ResourceHandle m_BloomMaskPS;
        ResourceHandle m_BloomCombinePS;
        ResourceHandle m_AO_PS;
        ResourceHandle m_AOBlur_PS;
        ResourceHandle m_BloomUpscalePS;
        ResourceHandle m_BloomDownscalePS;
        ResourceHandle m_BloomDownscaleKarisPS;
        ResourceHandle m_AONormals;

    public:
        // Inherited via PostProcessEffectBase
        virtual void Initialize(ID3D11Device1* device, ID3D11DeviceContext1* context, D3D11_TEXTURE2D_DESC* desc) override;
        virtual void Render(ID3D11ShaderResourceView** inSRV, ID3D11RenderTargetView** outRTV) override;
        virtual void Shutdown() override;
};