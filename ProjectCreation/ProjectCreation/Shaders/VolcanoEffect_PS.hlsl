#include "Samplers.hlsl"

#include "DefaultPixelIn.hlsl"
#include "DefaultVertexIn.hlsl"

texture2D tex : register(t0);
texture2D diffuse : register(t1);
texture2D normals : register(t2);

float4    main(INPUT_PIXEL input) : SV_TARGET
{
        float4 output = (1.0f, 1.0f, 1.0f, 1.0f);
        output        = tex.Sample(sampleTypeWrap, input.Tex);
        return output;
}