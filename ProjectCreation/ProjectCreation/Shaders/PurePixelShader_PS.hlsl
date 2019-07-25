struct PS_INPUT
{
        float4 pos : SV_POSITION;
        float2 uv : TEXCOORD;
        float3 norm : NORMAL;
        float4 color : COLOR;
};

cbuffer TextureInfo : register(b0)
{

};
Texture2D    tex1 : register(t0);
SamplerState objSamplerState : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{

        float4 output = input.color;
        output        = tex1.Sample(objSamplerState, input.uv).xyzx;
		//Transparency
        output.xyz *= output.a; // pre multiply alpha
		//clip(output.a > 0.1);

        return output;
}