struct PS_INPUT
{
        float4 pos : SV_POSITION;
        float2 uv : TEXCOORD;
        float3 norm : NORMAL;
        float4 color : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET
{

        float4 output = input.color;
        output        = float4(1.0f, 0.0f, 0.0f, 1.0f);
        return output;
}