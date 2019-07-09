struct PS_INPUT
{
        float4 pos : SV_POSITION;
        float2 uv : TEXCOORD;
        float3 norm : NORMAL;
        float4 color : COLOR;
};

Texture2D<float> tex;

float4 main(PS_INPUT input) : SV_TARGET
{

        float4 output = input.color;
        output        = tex.Sample(TextureSampler, input);
        //output        = float4(5.0f, 0.0f, 0.0f, 1.0f);
        return output;
}