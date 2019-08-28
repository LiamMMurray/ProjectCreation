#include "BloomInclude.hlsl"
#include "PostProcessConstantBuffers.hlsl"

#include "Math.hlsl"
#include "Samplers.hlsl"

Texture2D RandomAO : register(t3);
Texture2D ScreenDepth : register(t1);

static const float ao_sample_rad = 0.5f;
static const float ao_intensity  = 6.0f;
static const float ao_scale      = 1.0f;
static const float ao_bias       = 0.3f;

float3 VSPositionFromDepth(float2 vTexCoord)
{
        // Get the depth value for this pixel
        float z = ScreenDepth.Sample(sampleTypeClamp, vTexCoord).r;
        // Get x/w and y/w from the viewport position
        float  x             = vTexCoord.x * 2 - 1;
        float  y             = (1 - vTexCoord.y) * 2 - 1;
        float4 vProjectedPos = float4(x, y, z, 1.0f);
        // Transform by the inverse projection matrix
        float4 vPositionVS = mul(vProjectedPos, _invProj);
        // Divide by w to get the view-space position
        return vPositionVS.xyz / vPositionVS.w;
}

float2 GetRandomNormalFromTexture(in float2 uv, float2 screenSize)
{
        return normalize(RandomAO.Sample(sampleTypeWrap, screenSize * uv / 64).xy * 2.0f - 1.0f);
}

float SampleAO(in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm)
{
        float3       diff = VSPositionFromDepth(tcoord + uv) - p;
        const float3 v    = normalize(diff);
        const float  d    = length(diff) * ao_scale;
        return max(0.0, dot(cnorm, v) - ao_bias) * (1.0 / (1.0 + d)) * ao_intensity;
}

float main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD0) : SV_TARGET0
{
        float  depthSample = ScreenDepth.Sample(sampleTypeClamp, texCoord).r;
        float3 viewPos     = VSPositionFromDepth(texCoord);
        float3 worldPos    = mul(float4(viewPos, 1.0f), _invView).xyz;
        float  linearDepth = viewPos.z;
        // return worldPos.xyzz / 100.0f;

        float2 screenSize  = float2(1.0f / _inverseScreenDimensions.x, 1.0f / _inverseScreenDimensions.y);
        float  aspectRatio = (screenSize.y / screenSize.x);


        float2 randOffset = float2(3.0f, 3.0f);

        float3 posVS    = viewPos;
        float3 posVS1   = VSPositionFromDepth(texCoord + float2(_inverseScreenDimensions.x, 0.0f));
        float3 posVS2   = VSPositionFromDepth(texCoord + float2(0.0f, _inverseScreenDimensions.y));
        float3 normalVS = normalize(cross(posVS1 - posVS, posVS2 - posVS));
        float3 normalWS = mul(float4(normalVS, 1.0f), _invView).xyz;
        float  rad      = ao_sample_rad / posVS.z;

        float2 randomVec = GetRandomNormalFromTexture(texCoord, screenSize);

        float ao = 0.0f;

        const float2 vec[4] = {float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1)};


        int sampleCount = 4;

		bool check = (linearDepth < 300.0f) && (worldPos.y < 30.0f);

        if (check)
                [unroll] for (int i = 0; i < sampleCount; ++i)
                {
                        float2 coord1 = reflect(vec[i], randomVec) * rad;
                        float2 coord2 = float2(coord1.x * 0.707 - coord1.y * 0.707, coord1.x * 0.707 + coord1.y * 0.707);

                        ao += SampleAO(texCoord, coord1 * 0.25, posVS, normalVS);
                        ao += SampleAO(texCoord, coord2 * 0.5, posVS, normalVS);
                        ao += SampleAO(texCoord, coord1 * 0.75, posVS, normalVS);
                        ao += SampleAO(texCoord, coord2, posVS, normalVS);
                }
        else
        {
                ao = 0.0f;
        }

        ao /= 16.0f;
        // return 1.0f - depthDelta;
        // return ao;

        return 1.0f - ao;
}