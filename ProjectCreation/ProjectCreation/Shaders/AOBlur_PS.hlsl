#include "Samplers.hlsl"
#include "BloomInclude.hlsl"

Texture2D ScreenTexture : register(t0);

float main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD0) : SV_TARGET0
{
        float2 txlSize = float2(_inverseScreenDimensions.x, _inverseScreenDimensions.y);
        float  result  = 0.0;
        for (int x = -1; x <= 1; ++x)
        {
                for (int y = -1; y <= 1; ++y)
                {
                        float2 offset = float2(float(x), float(y)) * txlSize;
                        result += ScreenTexture.Sample(sampleTypeClamp, texCoord + offset).r;
                }
        }
        return result / (3.0 * 3.0);
}