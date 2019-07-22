#include "SceneBuffer.hlsl"

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
        float3 gOriginOffset;
        float  gTerrainAlpha;
};

Texture2D HeightMap : register(t9);
