cbuffer BloomCB : register(b0)
{
        float  _threshold;
        float  _brightness;
        float  _blurStrength;
        float  _blurRadius;
        float2 _inverseScreenDimensions;
};