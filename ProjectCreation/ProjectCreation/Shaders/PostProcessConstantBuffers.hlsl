
cbuffer CScreenSpaceBuffer : register(b0)
{
        matrix _invProj;
        matrix _invView;
        matrix _Proj;
        float3 _playerPosition;
        float  _time;
        float3 _selectionColor;
        float  _selectionAlpha;
};