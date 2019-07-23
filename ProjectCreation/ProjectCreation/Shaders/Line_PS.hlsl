cbuffer CSurfaceProperties : register(b2)
{
        float3 _diffuseColor;
        float  _ambientIntensity;
        float3 _emissiveColor;
        float  _specular;
        float  _roughnessMin;
        float  _roughnessMax;
        float  _metallic;
        float  _normalIntensity;
        int    _textureFlags;
};

struct INPUT_PIXEL
{
        float4 Pos : SV_POSITION;
};

float4 main(INPUT_PIXEL pIn) : SV_TARGET
{
        return _emissiveColor.xyzz;
}