
cbuffer CScreenSpaceBuffer : register(b0)
{
        matrix _invProj;
        matrix _invView;
        float3 _playerPosition;
        float  _time;
};