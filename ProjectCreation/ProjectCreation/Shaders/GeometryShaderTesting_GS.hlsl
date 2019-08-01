#include "MVPBuffer.hlsl"
#include "SceneBuffer.hlsl"

struct FParticleGPU
{
        float4 position;
        float4 prevPos;
        float4 color;
        float3 velocity;
        float2 uv;
        float  time;
        float  lifeSpan;
        int    emitterIndex;
        float  scale;
        float3 acceleration;
};

struct FTextureSetting
{
        int index;
        int col;
        int row;
};
StructuredBuffer<FParticleGPU> buffer : register(t1);
// StructuredBuffer<FTextureSetting> Texbuffer : register(t2);

struct VS_OUTPUT
{
        uint id : ID;
};

struct GSOutput
{
        float4 pos : SV_POSITION;
        float3 posWS : POSITION;
        float2 uv : TEXCOORD;
        float3 norm : NORMAL;
        float4 color : COLOR;
};

void TextureAddress(inout float2 UVcord[4], int index, int rowcol) // row and coll must be the same
{
        // index is the number texture you want to get. (Starts from 0!)
        float cellSize;
        // float cellSizeY;
        int cells; // amount of cells
        cells    = (rowcol - 1) * 2;
        cellSize = 1 / (rowcol - 1);
        // cellSizeY = 1 / (col - 1);

        if (index <= (cells - 1) || index >= 0)
        {
                int   rowX   = fmod(index, rowcol);
                int   colY   = index / rowcol;
                float xValue = rowX * cellSize;
                float yValue = colY * cellSize;
                UVcord[0]    = float2(xValue, yValue);                       // top left
                UVcord[1]    = float2(xValue + cellSize, yValue);            // top right
                UVcord[2]    = float2(xValue, yValue + cellSize);            // bottom left
                UVcord[3]    = float2(xValue + cellSize, yValue + cellSize); // bottom left
        }
}

[maxvertexcount(4)] void main(point VS_OUTPUT input[1], inout TriangleStream<GSOutput> output, uint InstanceID
                              : SV_PrimitiveID) {
        GSOutput verts[4] = {(GSOutput)0, (GSOutput)0, (GSOutput)0, (GSOutput)0};

        if (buffer[InstanceID].time > 0.0f)
        {

                for (int i = 0; i < 4; ++i)
                {
                        verts[i].posWS = buffer[InstanceID].position;
                        // verts[i].pos   = float4(0.0f, 0.0f, 0.0f, 1.0f);
                        verts[i].color = buffer[InstanceID].color;
                }

                uint numStructs = 0;
                uint stride     = 0;
                buffer.GetDimensions(numStructs, stride);


                float scale = buffer[InstanceID].scale;
                // scale       = 1.0f;
                // verts[j].pos = mul(float4(verts[j].pos.xyz, 1.0f), World);
                verts[0].pos = mul(float4(verts[0].posWS, 1.0f), ViewProjection);
                verts[0].pos = verts[0].pos + float4(-1.0f / _AspectRatio, 1.0f, 0.0f, 0.0f) * scale;
                verts[0].uv  = float2(0.0f, 0.0f);

                verts[1].pos = mul(float4(verts[1].posWS, 1.0f), ViewProjection);
                verts[1].pos = verts[1].pos + float4(1.0f / _AspectRatio, 1.0f, 0.0f, 0.0f) * scale;
                verts[1].uv  = float2(1.0f, 0.0f);


                verts[2].pos = mul(float4(verts[2].posWS, 1.0f), ViewProjection);
                verts[2].pos = verts[2].pos + float4(-1.0f / _AspectRatio, -1.0f, 0.0f, 0.0f) * scale;
                verts[2].uv  = float2(0.0f, 1.0f);


                verts[3].pos = mul(float4(verts[3].posWS, 1.0f), ViewProjection);
                verts[3].pos = verts[3].pos + float4(1.0f / _AspectRatio, -1.0f, 0.0f, 0.0f) * scale;
                verts[3].uv  = float2(1.0f, 1.0f);

                //TextureAddress(verts.uv, int index, int rowcol);

                for (int j = 0; j < 4; ++j)
                {
                        output.Append(verts[j]);
                }
        }
}
