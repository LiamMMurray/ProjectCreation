cbuffer SceneInfoBuffer : register(b1)
{
        float3 _EyePosition;
        float  _Time;
        float3 _DirectionalLightDirection;
        float  _playerRadius;
        float3 _DirectionalLightColor;
        float  _AspectRatio;
        float3 _AmbientColor;
        float  _DeltaTime;
        float  _Scale;
};