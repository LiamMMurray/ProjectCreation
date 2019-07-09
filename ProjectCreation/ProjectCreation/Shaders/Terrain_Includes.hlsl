

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
        matrix WorldView;
        float4 gWorldFrustumPlanes[6];
        float  gTexScale;
        float  gTexelSize;
        float  gScale;
        float  gWorldCellSpace;
        float2 screenDimensions;
        float  triangleSize;
        float  cellSizeWorld;
        float  gTerrainAlpha;
};

Texture2D HeightMap : register(t9);
