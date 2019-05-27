#include "Samplers.hlsl"

#include "BloomInclude.hlsl"

Texture2D ScreenTexture : register(t0);

float4 BoxFilter(float4 p0, float4 p1, float4 p2, float4 p3)
{
        return (p0 + p1 + p2 + p3) * 0.25f;
}

/** Based off Brian Karis's Average method for Unreal Engine, implemented in Advanced Warfare **/
float Luma(float3 c)
{
        return dot(c, half3(0.2126, 0.7152, 0.0722));
}


float4 main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD0) : SV_TARGET0
{
        float2 txlSize = float2(_inverseScreenDimensions.x, _inverseScreenDimensions.y);

        float4 c0  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(-1.0f, -1.0f) * txlSize);
        float4 c1  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(0.0f, -1.0f) * txlSize);
        float4 c2  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(1.0f, -1) * txlSize);
        float4 c3  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(-0.5, -0.5) * txlSize);
        float4 c4  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(0.5, -0.5) * txlSize);
        float4 c5  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(-1, 0) * txlSize);
        float4 c6  = ScreenTexture.Sample(sampleTypeClamp, texCoord);
        float4 c7  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(1, 0) * txlSize);
        float4 c8  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(-0.5, 0.5) * txlSize);
        float4 c9  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(0.5, 0.5) * txlSize);
        float4 c10 = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(-1, 1) * txlSize);
        float4 c11 = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(0, 1) * txlSize);
        float4 c12 = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(1, 1) * txlSize);

        float4 block1 = BoxFilter(c0, c1, c5, c6);
        float4 block2 = BoxFilter(c1, c2, c6, c7);
        float4 block3 = BoxFilter(c5, c6, c10, c11);
        float4 block4 = BoxFilter(c6, c7, c11, c12);
        float4 block5 = BoxFilter(c3, c4, c8, c9);

        float w1 = 0.125f * 1 / (Luma(block1.rgb) + 1);
        float w2 = 0.125f * 1 / (Luma(block2.rgb) + 1);
        float w3 = 0.125f * 1 / (Luma(block3.rgb) + 1);
        float w4 = 0.125f * 1 / (Luma(block4.rgb) + 1);
        float w5 = 0.500f * 1 / (Luma(block4.rgb) + 1);

        float inv_sum_w = 1 / (w1 + w2 + w3 + w4 + w5);

        return (w1 * block1 + w2 * block2 + w3 * block3 + w4 * block4 + w5 * block5) * inv_sum_w;
}
