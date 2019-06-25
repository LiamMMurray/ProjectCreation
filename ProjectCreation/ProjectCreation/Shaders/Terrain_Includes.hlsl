

cbuffer SceneInfoBuffer : register(b1)
{
        float3 _EyePosition;
        float  _Time;
        float3 _DirectionalLightDirection;
        float  _playerRadius;
        float3 _DirectionalLightColor;
        // float pad
        float3 _AmbientColor;
};

cbuffer TerrainInfoBuffer : register(b4)
{
        float4 gWorldFrustumPlanes[6];
        float  gTexScale;
        float _TesselationFactor;
        float  gTexelCellSpaceU;
        float  gTexelCellSpaceV;
        float  gWorldCellSpace;
};

Texture2D HeightMap : register(t9);
