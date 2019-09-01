#pragma once

#include <DirectXMath.h>
#include "../../MathLibrary/Transform.h"
#include "../../ResourceManager/IResource.h"
#include <D3DNativeTypes.h>
#include <InstanceData.h>
class RenderSystem;

struct ID3D11HullShader;
struct ID3D11DomainShader;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

struct CTerrainInfoBuffer
{
        DirectX::XMMATRIX worldView;
        DirectX::XMFLOAT4 gWorldFrustumPlanes[6];
        float             gTexScale;
        float             gTexelSize;
        float             gScale;
        float             gWorldCellSpace;
        DirectX::XMFLOAT2 gScreenDimensions;
        float             gTriangleSize;
        float             gCellSizeWorld;
        DirectX::XMFLOAT3 gOriginOffset;
        float             gTerrainAlpha;
};

struct TerrainVertex
{
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 tex;
        DirectX::XMFLOAT2 boundsY;
};

struct FInstanceRenderData
{
        ResourceHandle        material;
        ResourceHandle        mesh;
        uint32_t              instanceCount;
        std::vector<uint32_t> instanceIndexList;
};


class TerrainManager
{
        static constexpr float WaterLevel = -1260.0f;

        static TerrainManager* instance;

        std::vector<ComponentHandle> staticMeshesShowWithTerrain;

        void _initialize(RenderSystem* rs);
        void _update(float deltaTime);
        void _shutdown();


        TerrainManager()  = default;
        ~TerrainManager() = default;

        RenderSystem* renderSystem;

        ID3D11InputLayout*  inputLayout;
        ID3D11VertexShader* vertexShader;
        ID3D11HullShader*   hullShader;
        ID3D11HullShader*   oceanHullShader;
        ID3D11DomainShader* domainShader;
        ID3D11DomainShader* oceanDomainShader;
        ID3D11PixelShader*  pixelShader;
        ID3D11PixelShader*  oceanPixelShader;

        ID3D11ShaderResourceView* terrainSourceSRV;
        ID3D11ShaderResourceView* terrainMaskSRV;
        ID3D11ShaderResourceView* terrainColorSRV;

        ID3D11Texture2D*          terrainIntermediateTexture;
        ID3D11RenderTargetView*   terrainIntermediateRenderTarget;
        ID3D11ShaderResourceView* terrainIntermediateSRV;

        ID3D11ShaderResourceView*  instanceSRV = nullptr;
        ID3D11UnorderedAccessView* instanceUAV = nullptr;

        ID3D11ShaderResourceView* instanceIndexSteepSRV = nullptr;
        ID3D11ShaderResourceView* instanceIndexFlatSRV  = nullptr;

        ID3D11UnorderedAccessView* instanceIndexSteepUAV = nullptr;
        ID3D11UnorderedAccessView* instanceIndexFlatUAV  = nullptr;

        ID3D11Buffer* indexCounterHelperBuffer = nullptr;

        ID3D11Buffer*    vertexBuffer;
        ID3D11Buffer*    indexBuffer;
        ID3D11Texture2D* stagingTextureResource;
        float*           terrainHeightArray;
        size_t           stagingTextureCPUElementCount;

        CTerrainInfoBuffer terrainConstantBufferCPU;
        ID3D11Buffer*      terrainConstantBufferGPU;

        static constexpr unsigned int gInstanceTransformsCount = 15000;
        FTransform                    m_InstanceTransforms[gInstanceTransformsCount];
        FInstanceData                 m_InstanceData[gInstanceTransformsCount];
        ResourceHandle                m_UpdateInstancesComputeShader;

        std::vector<FInstanceRenderData> instanceDrawCallsDataFlat;
        std::vector<FInstanceRenderData> instanceDrawCallsDataSteep;

        void GenerateInstanceTransforms(FTransform tArray[gInstanceTransformsCount]);
        void WrapInstanceTransforms();

        void CreateVertexBuffer(ID3D11Buffer** buffer, unsigned int squareDimensions, float waterLevel, float scale);
        void CreateIndexBuffer(ID3D11Buffer** buffer, unsigned int squareDimensions);

        unsigned int patchQuadCount;
        unsigned int patchSquareDimensions;
        unsigned int patchCells = 16;
        unsigned int textureDimensions;

        unsigned int intermediateMipLevel = 3;
        unsigned int intermediateMipDimensions;

        ResourceHandle mTextureHandles[6];

        float scale        = 0.06f;
        float groundOffset = 3.0f;

        DirectX::XMMATRIX TerrainMatrix;
        DirectX::XMMATRIX InverseTerrainMatrix;

    public:
        inline float GetGroundOffset() const
        {
                return groundOffset;
        }
        static TerrainManager* Get();


        DirectX::XMVECTOR AlignPositionToTerrain(const DirectX::XMVECTOR& pos);
        static void       Initialize(RenderSystem* rs);
        static void       Update(float deltaTime);
        static void       Shutdown();

        inline float GetScale()
        {
                return 8000.0f * scale;
        }
};