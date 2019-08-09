#include "MVPBuffer.hlsl"
#include "ParticleData.hlsl"
#include "SceneBuffer.hlsl"

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


float Round(float input)
{
        float output = (int)(input * 100 + .5);
        return (float)output / 100;
}
void TextureAddress(inout float2 UVcord[4], uint index, uint rowcol) // row and coll must be the same
{
        // index is the number texture you want to get. (Starts from 0!)
        float cellSize;
        // float cellSizeY;
        uint cells; // amount of cells
        cells    = rowcol * rowcol;
        cellSize = 1.0f / rowcol;
        // cellSizeY = 1 / (col - 1);

        uint  rowX   = index % rowcol;
        uint  colY   = index / rowcol;
        float xValue = rowX * cellSize;
        float yValue = colY * cellSize;
        UVcord[0]    = float2((xValue), (yValue));                       // top left
        UVcord[1]    = float2((xValue + cellSize), (yValue));            // top right
        UVcord[2]    = float2((xValue), (yValue + cellSize));            // bottom left
        UVcord[3]    = float2((xValue + cellSize), (yValue + cellSize)); // bottom right
}

[maxvertexcount(4)] void main(point VS_OUTPUT input[1], inout TriangleStream<GSOutput> output, uint InstanceID
                              : SV_PrimitiveID) {
        GSOutput verts[4] = {(GSOutput)0, (GSOutput)0, (GSOutput)0, (GSOutput)0};
        uint     id       = InstanceID;

        float2 uv[4] = {float2(0.0f, 0.0f), float2(1.0f, 0.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f)}; //defult value\

        TextureAddress(uv, (uint)buffer[InstanceID].textureIndex, 2); // buffer[InstanceID].textueRowCol

        float3 screenVel = mul(float4(buffer[InstanceID].velocity.xyz, 0.0f), ViewProjection).xyz;

        float3 zValue = {0.0f, 0.0f, 1.0f};

        float dirAlpha = float(buffer[InstanceID].flags & ALIGN_TO_VEL);

        float3 upDir    = lerp(float3(0.0f, 1.0f, 0.0f), normalize(screenVel.xyz), dirAlpha); // vel direction
        float3 rightDir = cross(upDir, zValue);

        if (buffer[InstanceID].time > 0.0f)
        {
                // Compute current data from initial and target
                float  timeAlpha    = 1.0f - buffer[id].time / buffer[id].lifeSpan.x;
                float4 currentColor = lerp(buffer[id].initialColor, buffer[id].finalColor, timeAlpha);
                float  currentScale = lerp(buffer[id].scale.x, buffer[id].scale.y, timeAlpha);

                if (buffer[id].lifeSpan.y > 0)
                        currentColor.a = lerp(0.0f, currentColor.a, saturate(timeAlpha / buffer[id].lifeSpan.y));

                if (buffer[id].lifeSpan.z > 0)
                        currentColor.a = lerp(0.0f, currentColor.a, saturate((1.0f - timeAlpha) / buffer[id].lifeSpan.z));

                // end compute current data

                for (int i = 0; i < 4; ++i)
                {
                        verts[i].posWS = buffer[InstanceID].position;
                        // verts[i].pos   = float4(0.0f, 0.0f, 0.0f, 1.0f);
                        verts[i].color = currentColor;
                }

                uint numStructs = 0;
                uint stride     = 0;
                buffer.GetDimensions(numStructs, stride);


                float  scale     = currentScale;
                float3 xScaleFix = float3(1.0f / _AspectRatio, -1.0f / _AspectRatio, 1.0f);
                // scale       = 1.0f;
                verts[0].pos = mul(float4(verts[0].posWS, 1.0f), ViewProjection);
                verts[0].pos.xyz += (-rightDir + upDir) * scale * xScaleFix.xzz;
                // verts[0].pos = verts[0].pos + (float4((-1.0f / _AspectRatio), 1.0f, 0.0f, 0.0f)) * scale;
                verts[0].uv = uv[0];
                // verts[0].normal;

                verts[1].pos = mul(float4(verts[1].posWS, 1.0f), ViewProjection);
                verts[1].pos.xyz += (rightDir + upDir) * scale * xScaleFix.xzz;
                // verts[1].pos = verts[1].pos + (float4((1.0f / _AspectRatio), 1.0f, 0.0f, 0.0f)) * scale;
                verts[1].uv = uv[1];


                verts[2].pos = mul(float4(verts[2].posWS, 1.0f), ViewProjection);
                verts[2].pos.xyz += (-rightDir - upDir) * scale * xScaleFix.xzz;
                // verts[2].pos = verts[2].pos + (float4((-1.0f / _AspectRatio), -1.0f, 0.0f, 0.0f)) * scale;
                verts[2].uv = uv[2];


                verts[3].pos = mul(float4(verts[3].posWS, 1.0f), ViewProjection);
                // verts[3].pos = verts[3].pos + (float4((1.0f / _AspectRatio), -1.0f, 0.0f, 0.0f)) * scale;

                verts[3].pos.xyz += (rightDir - upDir) * scale * xScaleFix.xzz;

                verts[3].uv = uv[3];

                // TextureAddress(verts.uv, int index, int rowcol);

                for (int j = 0; j < 4; ++j)
                {
                        output.Append(verts[j]);
                }
        }
}
