struct FParticleGPU
{
        float4 position;
        float4 color;
        float3 velocity;
        float2 uv;
        float  time;
        bool   active;
        int    index;
};

cbuffer MVPBuffer : register(b0)
{
        matrix ViewProjection;
        matrix World;
};

cbuffer SceneInfoBuffer : register(b1)
{
        float3 _EyePosition;
        float  _Time;
        float3 _DirectionalLightDirection;
        float  _playerRadius;
        float3 _DirectionalLightColor;
        float  aspectRatio;
        float3 _AmbientColor;
};

StructuredBuffer<FParticleGPU> buffer : register(t1);

struct VS_OUTPUT
{
        uint id : ID;
};

struct GSOutput
{
        float4 pos : SV_POSITION;
        float2 uv : TEXCOORD;
        float3 norm : NORMAL;
        float4 color : COLOR;
};

[maxvertexcount(4)] void main(point VS_OUTPUT input[1], inout TriangleStream<GSOutput> output, uint InstanceID
                              : SV_PrimitiveID) {
        GSOutput verts[4] = {(GSOutput)0, (GSOutput)0, (GSOutput)0, (GSOutput)0};

        for (int i = 0; i < 4; ++i)
        {
                        verts[i].pos = buffer[InstanceID].position;
                        // verts[i].pos   = float4(0.0f, 0.0f, 0.0f, 1.0f);
                        verts[i].color = buffer[InstanceID].color;
        }

        uint numStructs = 0;
        uint stride     = 0;
        buffer.GetDimensions(numStructs, stride);

        // verts[j].pos = mul(float4(verts[j].pos.xyz, 1.0f), World);
        verts[0].pos = mul(float4(verts[0].pos.xyz, 1.0f), ViewProjection);
         verts[0].pos = verts[0].pos + float4(-1.0f / aspectRatio, 1.0f, 0.0f, 0.0f);

        verts[1].pos = mul(float4(verts[1].pos.xyz, 1.0f), ViewProjection);
         verts[1].pos = verts[1].pos + float4(1.0f / aspectRatio, 1.0f, 0.0f, 0.0f);

        verts[2].pos = mul(float4(verts[2].pos.xyz, 1.0f), ViewProjection);
         verts[2].pos = verts[2].pos + float4(-1.0f / aspectRatio, -1.0f, 0.0f, 0.0f);

        verts[3].pos = mul(float4(verts[3].pos.xyz, 1.0f), ViewProjection);
         verts[3].pos = verts[3].pos + float4(1.0f / aspectRatio, -1.0f, 0.0f, 0.0f);

        for (int j = 0; j < 4; ++j)
        {
                output.Append(verts[j]);
        }
}