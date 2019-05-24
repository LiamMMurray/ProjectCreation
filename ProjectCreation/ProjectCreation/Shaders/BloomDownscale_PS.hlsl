#include "Samplers.hlsl"

#include "BloomInclude.hlsl"

Texture2D ScreenTexture : register(t0);

float4 BoxFilter(float4 p0, float4 p1, float4 p2, float4 p3)
{
        return (p0 + p1 + p2 + p3) * 0.25f;
}

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

        return BoxFilter(c0, c1, c5, c6) * 0.125f + BoxFilter(c1, c2, c6, c7) * 0.125f + BoxFilter(c5, c6, c10, c11) * 0.125f +
               BoxFilter(c6, c7, c11, c12) * 0.125f + BoxFilter(c3, c4, c8, c9) * 0.5f;
}
