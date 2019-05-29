cbuffer BloomCB : register(b10)
{
        float  _threshold;
        float  _brightness;
        float  _blurRadius;
        unsigned int _hOrV;
        float2 _inverseScreenDimensions;
};