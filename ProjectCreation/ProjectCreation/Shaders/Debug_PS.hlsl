struct INPUT_PIXEL
{
        float4 Pos : SV_POSITION;
        float4 Color : COLOR;
};

float4 main(INPUT_PIXEL pIn) : SV_TARGET
{
        return pIn.Color;
}