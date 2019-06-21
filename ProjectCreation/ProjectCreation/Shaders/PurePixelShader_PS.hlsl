struct PS_INPUT
{
        float4 pos : SV_POSITION;
        float4 color : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{

        float4 output = input.color;
        return output;
}