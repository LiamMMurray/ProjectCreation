struct PS_INPUT
{
        float4 color;
};

float4 main(PS_INPUT input) : SV_TARGET
{

        float4 output = input.color;
        return output;
}