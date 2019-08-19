struct PS_INPUT
{};
texture2D tex : register(t0);

float4    main(PS_INPUT input) : SV_TARGET
{
        float4 output = (1.0f, 1.0f, 1.0f, 1.0f);
        return output;
}