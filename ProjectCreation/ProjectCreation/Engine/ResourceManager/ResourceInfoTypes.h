#pragma once

#include <DirectXMath.h>

#define SURFACE_FLAG_HAS_DIFFUSE_MAP 0b1
#define SURFACE_FLAG_HAS_METALLIC_MAP 0b10
#define SURFACE_FLAG_HAS_ROUGHNESS_MAP 0b100
#define SURFACE_FLAG_HAS_NORMAL_MAP 0b1000
#define SURFACE_FLAG_HAS_AO_MAP 0b10000
#define SURFACE_FLAG_HAS_EMISSIVE_MASK 0b100000
#define SURFACE_FLAG_IS_MASKED 0b10000000
#define SURFACE_FLAG_IS_TRANSLUSCENT 0b100000000
#define SURFACE_FLAG_IS_UNLIT 0b1000000000

#define SURFACE_FLAG_HAS_DETAILS_MAP (SURFACE_FLAG_HAS_ROUGHNESS_MAP | SURFACE_FLAG_HAS_METALLIC_MAP | SURFACE_FLAG_HAS_AO_MAP)


enum ETexture2DType
{
        Diffuse  = 0,
        Normal   = 1,
        Details  = 2,
        Emissive = 3,
        COUNT
};


enum class EMaterialType
{
        PBRStatic  = 0,
        PBRSkinned = 1
};


struct FSurfaceProperties
{
        DirectX::XMFLOAT3 diffuseColor     = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
        float             ambientIntensity = 1.0f;
        DirectX::XMFLOAT3 emissiveColor    = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        float             specular         = 0.04f;
        float             roughnessMin     = 0.0f;
        float             roughnessMax     = 1.0f;
        float             metallic         = 1.0f;
        float             normalIntensity  = 1.0f;
        int               textureFlags     = 0;
        DirectX::XMFLOAT3 padding          = {};

        inline bool IsTransluscent() const
        {
                return textureFlags & SURFACE_FLAG_IS_TRANSLUSCENT;
        }

        inline bool IsMasked() const
        {
                return textureFlags & SURFACE_FLAG_IS_MASKED;
        }
};