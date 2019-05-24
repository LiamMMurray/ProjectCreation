#pragma once
#include "PostProcessEffectBase.h"

#include <DirectXMath.h>
#include <stdint.h>

#include "../../Engine/ResourceManager/IResource.h"

struct E_BLOOM_PS_SRV
{
        enum
        {
                BLOOM = 0,
                SCREEN,
                COUNT
        };
};


struct alignas(16) BloomCB
{
        float             threshold = 0.8f;
        float             brightness = 1.0f;
        float             blurStrength;
        float             blurRadius;
        DirectX::XMFLOAT2 inverseScreenDimensions;
};

class ResourceManager;

class Bloom : public PostProcessEffectBase
{
        static constexpr uint8_t BloomPasses = 6;

        struct E_PASSES
        {
                enum
                {
                        MASK = Bloom::BloomPasses,
                        COUNT
                };
        };
        static constexpr uint8_t TotalPasses = E_PASSES::COUNT;

        static constexpr float m_Strengths[5] = {0.8f, 1.0f, 0.9f, 0.6f, 0.6f};
        static constexpr float m_Radiuses[5]  = {2.0f, 2.0f, 2.0f, 2.0f, 4.0f};

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
        ResourceHandle m_BloomUpscalePS;
        ResourceHandle m_BloomDownscalePS;

    public:
        // Inherited via PostProcessEffectBase
        virtual void Initialize(ID3D11Device1* device, ID3D11DeviceContext1* context, D3D11_TEXTURE2D_DESC* desc) override;
        virtual void Render(ID3D11ShaderResourceView** inSRV, ID3D11RenderTargetView** outRTV) override;
        virtual void Shutdown() override;
};