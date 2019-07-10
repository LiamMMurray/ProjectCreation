struct PS_INPUT
{
        float4 pos : SV_POSITION;
        float2 uv : TEXCOORD;
        float3 norm : NORMAL;
        float4 color : COLOR;
};

Texture2D tex : register (t0);
SamplerState     objSamplerState : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{

        float4 output = input.color;
		output        = tex.Sample(objSamplerState, input.uv);
        
        return output;
}