#pragma once

#include <DirectXMath.h>
#include "..//..//Engine/ResourceManager/IResource.h"
#include "..//..//Utility/ForwardDeclarations/D3DNativeTypes.h"
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
        float             gTerrainAlpha;
        DirectX::XMFLOAT3 padding;
};

struct TerrainVertex
{
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 tex;
};

class TerrainManager
{
        static constexpr float WaterLevel = -1260.0f;

        static TerrainManager* instance;

        void _initialize(RenderSystem* rs);
        void _update(float deltaTime);
        void _shutdown();


        TerrainManager()  = default;
        ~TerrainManager() = default;

        RenderSystem* renderSystem;

        ID3D11InputLayout*  inputLayout;
        ID3D11VertexShader* vertexShader;
        ID3D11HullShader*   hullShader;
        ID3D11DomainShader* domainShader;
        ID3D11DomainShader* oceanDomainShader;
        ID3D11PixelShader*  pixelShader;
        ID3D11PixelShader*  oceanPixelShader;

        ID3D11Texture2D*          terrainSourceTexture;
        ID3D11ShaderResourceView* terrainSourceSRV;

        ID3D11Texture2D*          terrainIntermediateTexture;
        ID3D11RenderTargetView*   terrainIntermediateRenderTarget;
        ID3D11ShaderResourceView* terrainIntermediateSRV;

        ID3D11Buffer*    vertexBuffer;
        ID3D11Buffer*    indexBuffer;
        ID3D11Texture2D* stagingTextureResource;
        float*           terrainHeightArray;
        size_t           stagingTextureCPUElementCount;

        CTerrainInfoBuffer terrainConstantBufferCPU;
        ID3D11Buffer*      terrainConstantBufferGPU;

        void CreateVertexBuffer(ID3D11Buffer** buffer, unsigned int squareDimensions, float waterLevel, float scale);
        void CreateIndexBuffer(ID3D11Buffer** buffer, unsigned int squareDimensions);

        unsigned int patchQuadCount;
        unsigned int patchSquareDimensions;
        unsigned int patchCells = 32
			
			;
        unsigned int textureDimensions;

        unsigned int intermediateMipLevel = 3;
        unsigned int intermediateMipDimensions;

        ResourceHandle mTextureHandles[6];

        float scale        = 0.06f;
        float groundOffset = 3.0f;

        DirectX::XMMATRIX TerrainMatrix;
        DirectX::XMMATRIX InverseTerrainMatrix;

    public:
        static TerrainManager* Get();
        DirectX::XMVECTOR      AlignPositionToTerrain(const DirectX::XMVECTOR& pos);
        static void            Initialize(RenderSystem* rs);
        static void            Update(float deltaTime);
        static void            Shutdown();

        inline float GetScale()
        {
                return 8000.0f * scale;
        }
};