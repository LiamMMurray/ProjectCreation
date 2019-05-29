#include "Samplers.hlsl"

#include "BloomInclude.hlsl"

Texture2D ScreenTexture : register(t0);

float4 BoxFilter(float4 p0, float4 p1, float4 p2, float4 p3)
{
        return (p0 + p1 + p2 + p3) * 0.25f;
}

/** Based off Brian Karis's Average method for Unreal Engine, implemented in Advanced Warfare **/

float4 main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD0) : SV_TARGET0
{
        float2 offset = float2(_inverseScreenDimensions.x, _inverseScreenDimensions.y);

        float4 c0  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(-2.0f, -2.0f) * offset);
        float4 c1  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(0.0f, -2.0f) * offset);
        float4 c2  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(2.0f, -2) * offset);
        float4 c3  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(-1, -1) * offset);
        float4 c4  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(1, -1) * offset);
        float4 c5  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(-2, 0) * offset);
        float4 c6  = ScreenTexture.Sample(sampleTypeClamp, texCoord);
        float4 c7  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(2, 0) * offset);
        float4 c8  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(-1, 1) * offset);
        float4 c9  = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(1, 1) * offset);
        float4 c10 = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(-2, 2) * offset);
        float4 c11 = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(0, 2) * offset);
        float4 c12 = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(2, 2) * offset);

        float4 block1 = BoxFilter(c0, c1, c5, c6);
        float4 block2 = BoxFilter(c1, c2, c6, c7);
        float4 block3 = BoxFilter(c5, c6, c10, c11);
        float4 block4 = BoxFilter(c6, c7, c11, c12);
        float4 block5 = BoxFilter(c3, c4, c8, c9);

        float w1 = 0.125 * 1 / (max(block1.r, max(block1.g, block1.b)) + 1);
        float w2 = 0.125 * 1 / (max(block2.r, max(block2.g, block2.b)) + 1);
        float w3 = 0.125 * 1 / (max(block3.r, max(block3.g, block3.b)) + 1);
        float w4 = 0.125 * 1 / (max(block4.r, max(block4.g, block4.b)) + 1);
        float w5 = 0.5f * 1 / (max(block5.r, max(block5.g, block5.b)) + 1);

        float inv_sum_w = 1 / (w1 + w2 + w3 + w4 + w5);

        return (w1 * block1 + w2 * block2 + w3 * block3 + w4 * block4 + w5 * block5) * inv_sum_w;
}
