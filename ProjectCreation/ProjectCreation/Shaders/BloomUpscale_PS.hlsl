#include "BloomInclude.hlsl"
#include "Samplers.hlsl"


Texture2D ScreenTexture : register(t0);

float4 BoxFilter(float4 p0, float4 p1, float4 p2, float4 p3)
{
        return (p0 + p1 + p2 + p3) * 0.25f;
}

static const float weight[3] = {0.227027f, 0.1216216f, 0.016216f};


float4 main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD0) : SV_TARGET0
{
        /** Using tent filter **/

        float2 offset = float2(_inverseScreenDimensions.x, _inverseScreenDimensions.y) * _blurRadius;

        float4 c0 = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(-1, -1) * offset);
        float4 c1 = 2.0f * ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(0, -1) * offset);
        float4 c2 = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(1, -1) * offset);
        float4 c3 = 2.0f * ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(-1, 0) * offset);
        float4 c4 = 4.0f * ScreenTexture.Sample(sampleTypeClamp, texCoord);
        float4 c5 = 2.0f * ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(1, 0) * offset);
        float4 c6 = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(-1, 1) * offset);
        float4 c7 = 2.0f * ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(0, 1) * offset);
        float4 c8 = ScreenTexture.Sample(sampleTypeClamp, texCoord + float2(1, 1) * offset);

        float4 color = 0.0625f * (c0 + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8);

        return color;
}