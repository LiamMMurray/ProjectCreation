struct VS_INPUT
{
        float4 position : POSITION;
        float4 color : COLOR;
};
struct VS_OUTPUT
{
        float4 position : POSITION;
        float4 color : COLOR;
};

VS_OUTPUT main(VS_INPUT input) : SV_POSITION
{
        VS_OUTPUT output;
        output = input;
        return output;
}