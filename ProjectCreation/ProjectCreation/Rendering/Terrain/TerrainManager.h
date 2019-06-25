#pragma once

#include <DirectXMath.h>
#include "..//..//Utility/ForwardDeclarations/D3DNativeTypes.h"
class RenderSystem;

struct ID3D11HullShader;
struct ID3D11DomainShader;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

struct CTerrainInfoBuffer
{
        DirectX::XMFLOAT4 gWorldFrustumPlanes[6];
        float             gTexScale;
        float             _TesselationFactor;
        float             gTexelCellSpaceU;
        float             gTexelCellSpaceV;
        float             gWorldCellSpace;
        DirectX::XMFLOAT3 padding;
};

struct TerrainVertex
{
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 tex;
};

class TerrainManager
{

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
        ID3D11PixelShader*  pixelShader;

        ID3D11Texture2D*          terrainSourceTexture;
        ID3D11ShaderResourceView* terrainSourceSRV;

        ID3D11Texture2D*        terrainTargetTexture;
        ID3D11RenderTargetView* terrainTargetRenderTarget;

        ID3D11Buffer* vertexBuffer;
        ID3D11Buffer* indexBuffer;

        CTerrainInfoBuffer terrainConstantBufferCPU;
        ID3D11Buffer*      terrainConstantBufferGPU;

        void CreateVertexBuffer(ID3D11Buffer** buffer, unsigned int squareDimensions, float waterLevel, float scale);
        void CreateIndexBuffer(ID3D11Buffer** buffer, unsigned int squareDimensions);

        unsigned int patchQuadCount;
        unsigned int patchSquareDimensions;
        unsigned int patchCells = 64;

		float scale = 0.2f;

    public:
        static TerrainManager* Get();

        static void Initialize(RenderSystem* rs);
        static void Update(float deltaTime);
        static void Shutdown();
};