#include "SceneBuffer.hlsl"

struct PS_INPUT
{
        float4 pos : SV_POSITION;
        float3 posWS : POSITION;
        float2 uv : TEXCOORD;
        float3 norm : NORMAL;
        float4 color : COLOR;
};

cbuffer TextureInfo : register(b0){

                      };
Texture2D    tex1 : register(t0);
SamplerState objSamplerState : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
        float4 output;
        uint   width;
        uint   height;
        uint   mipLevel;
        uint   numOfLevel;
        output = tex1.Sample(objSamplerState, input.uv).xyzw;
        // tex1.GetDimensions(width, height);
        output *= input.color;
        // Transparency
        float dist = distance(_EyePosition, input.posWS);
        output.a   = saturate(output.a);
        output.a   = lerp(0.0f, output.a, saturate((dist - 1.0f) / 0.65f));

        output.xyz *= output.a; // pre multiply alpha
                                // clip(output.a > 0.1);


        return output;
}