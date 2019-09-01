#pragma once

/** Includes **/

#include <HandleManager.h>
#include <SystemManager.h>
#include <D3DNativeTypes.h>
#include <WinProcTypes.h>
#include <PostProcessEffectBase.h>

#include "../../ResourceManager/IResource.h"

#include "ConstantBuffers.h"

#include <DirectXMath.h>

#include "InstanceData.h"


/** Forward Declarations **/
struct StaticMesh;
struct SkeletalMesh;
struct Material;
struct FTransform;
struct FCameraSettings;

class ResourceManager;

namespace Animation
{
        struct FSkeleton;
}

/** Helper ENUMS **/

struct E_VIEWPORT
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct E_CONSTANT_BUFFER_BASE_PASS
{
        enum
        {
                MVP = 0,
                SCENE,
                SURFACE,
                ANIM,
                COUNT
        };
};

struct E_CONSTANT_BUFFER_POST_PROCESS
{
        enum
        {
                SCREENSPACE = 0,
                COUNT
        };
};

struct E_VERTEX_SHADER_RESOURCE_VIEW
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct E_BASE_PASS_PIXEL_SRV
{
        enum
        {
                DIFFUSE_MAP = 0,
                NORMAL_MAP,
                DETAILS_MAP,
                EMISSIVE_MAP,
                PER_MAT_COUNT,
                DIFFUSE_REFLECTION_MAP = PER_MAT_COUNT,
                SPECULAR_REFLECTION_MAP,
                INTEGRATION_MAP,
                MASK1,
                MASK2,
                DIRECTIONAL_SHADOW_MAP,
                POINT_SHADOW_MAP_1,
                POINT_SHADOW_MAP_2,
                SPOT_SHADOW_MAP_1,
                SPOT_SHADOW_MAP_2,

                COUNT
        };
};

struct E_VERTEX_SHADERS
{
        enum
        {
                DEFAULT = 0,
                SKINNED,
                DEBUG,
                LINE,
                COUNT
        };
};

struct E_PIXEL_SHADERS
{
        enum
        {
                DEFAULT = 0,
                DEBUG,
                LINE,
                COUNT
        };
};

struct E_POSTPROCESS_PIXEL_SRV
{
        enum
        {
                BASE_PASS = 0,
                BASE_DEPTH,
                COUNT
        };
};

struct E_SAMPLER_STATE
{
        enum
        {
                WRAP = 0,
                SHADOWS,
                CLAMP,
                NEAREST,
                SKY,
                COUNT
        };
};

struct E_RENDER_TARGET
{
        enum
        {
                BACKBUFFER = 0,
                BASE_PASS,
                POSITION_PASS,
                VELOCITY_PASS,

                COUNT
        };
};

struct E_INPUT_LAYOUT
{
        enum
        {
                DEFAULT = 0,
                SKINNED,
                DEBUG,
                LINE,
                COUNT
        };
};

struct E_RASTERIZER_STATE
{
        enum
        {
                DEFAULT = 0,
                WIREFRAME,
                LINE,
                COUNT
        };
};

struct E_DEPTH_STENCIL
{
        enum
        {
                BASE_PASS = 0,
                COUNT
        };
};

struct E_DEPTH_SHADOW
{
        enum
        {
                BASE_PASS = 0,
                COUNT
        };
};

struct E_BLEND_STATE
{
        enum
        {
                Opaque = 0,
                Transluscent,
                Additive,
                COUNT
        };
};

struct E_STATE_DEPTH_STENCIL
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct E_DEPTH_STENCIL_STATE
{
        enum
        {
                BASE_PASS,
                TRANSLUSCENT,
                COUNT
        };
};

struct FDraw
{
        enum class EDrawType
        {
                Static = 0,
                Skeletal
        } meshType;

        ResourceHandle    meshResource;
        ResourceHandle    materialHandle;
        ComponentHandle   componentHandle;
        DirectX::XMMATRIX mtx;
};


class RenderSystem : public ISystem
{
        friend class ResourceManager;
        friend class UIManager;
        friend class ParticleManager;
        friend class TerrainManager;

        ComponentHandle m_SkyHandle;
        ComponentHandle m_CloudHandle;

        std::vector<FDraw> m_OpaqueDraws;
        std::vector<FDraw> m_TransluscentDraws;

        IDXGISwapChain1*      m_Swapchain;
        ID3D11Device1*        m_Device;
        ID3D11DeviceContext1* m_Context;

        std::vector<PostProcessEffectBase*> m_PostProcessChain;

        void CreateDeviceAndSwapChain();
        void CreateDefaultRenderTargets(D3D11_TEXTURE2D_DESC* backbufferDesc = nullptr,
                                        unsigned int          width          = 0,
                                        unsigned int          height         = 0,
                                        bool                  dontOverride   = false);
        void CreateRasterizerStates();
        void CreateInputLayouts();
        void CreateCommonShaders();
        void CreateCommonConstantBuffers();
        void CreateSamplerStates();
        void CreateBlendStates();
        void CreateDepthStencilStates();
        void CreatePostProcessEffects(D3D11_TEXTURE2D_DESC* desc);

        /** Debug Renderer **/
        ID3D11Buffer* m_DebugVertexBuffer;
        ID3D11Buffer* m_LineVertexBuffer;
        void          CreateDebugBuffers();
        void          DrawDebug();
        void          DrawLines();

        ID3D11RenderTargetView*   m_DefaultRenderTargets[E_RENDER_TARGET::COUNT]{};
        ID3D11ShaderResourceView* m_PostProcessSRVs[E_POSTPROCESS_PIXEL_SRV::COUNT]{};
        ID3D11DepthStencilView*   m_DefaultDepthStencil[E_DEPTH_STENCIL::COUNT]{};
        ID3D11InputLayout*        m_DefaultInputLayouts[E_INPUT_LAYOUT::COUNT]{};
        ID3D11RasterizerState*    m_DefaultRasterizerStates[E_RASTERIZER_STATE::COUNT]{};
        ID3D11SamplerState*       m_DefaultSamplerStates[E_SAMPLER_STATE::COUNT]{};
        ID3D11BlendState*         m_BlendStates[E_BLEND_STATE::COUNT]{};
        ID3D11DepthStencilState*  m_DepthStencilStates[E_DEPTH_STENCIL_STATE::COUNT]{};

        ResourceHandle m_CommonVertexShaderHandles[E_VERTEX_SHADERS::COUNT];
        ResourceHandle m_CommonPixelShaderHandles[E_PIXEL_SHADERS::COUNT];
        ResourceHandle m_LineGeometryShader;


        /** Base pass constant buffers **/
        ID3D11Buffer*    m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::COUNT] = {};
        CTransformBuffer m_ConstantBuffer_MVP;
        CSceneInfoBuffer m_ConstantBuffer_SCENE;
        CAnimationBuffer m_ConstantBuffer_ANIM;


        DirectX::XMVECTOR currVel = DirectX::XMVectorZero();
        /** Post Process Constant Buffers **/
        ID3D11Buffer*      m_PostProcessConstantBuffers[E_CONSTANT_BUFFER_POST_PROCESS::COUNT] = {};
        CScreenSpaceBuffer m_ContstantBuffer_SCREENSPACE;

        float m_BackBufferWidth;
        float m_BackBufferHeight;

        void UpdateConstantBuffer(ID3D11Buffer* gpuBuffer, void* cpuBuffer, size_t size);

        void DrawMesh(ID3D11Buffer*      vertexBuffer,
                      ID3D11Buffer*      indexBuffer,
                      uint32_t           indexCount,
                      uint32_t           vertexSize,
                      Material*          material,
                      DirectX::XMMATRIX* mtx);


        void DrawStaticMesh(StaticMesh* mesh, Material* material, DirectX::XMMATRIX* mtx);
        void DrawSkeletalMesh(SkeletalMesh* mesh, Material* material, DirectX::XMMATRIX* mtx, const Animation::FSkeleton* skel);


        ComponentHandle  m_MainCameraHandle;
        ComponentHandle  m_MainTransformHandle;
        ResourceManager* m_ResourceManager;
        HandleManager*   m_HandleManager;

        DirectX::XMMATRIX m_CachedMainProjectionMatrix;
        DirectX::XMMATRIX m_CachedMainViewMatrix;
        DirectX::XMMATRIX m_CachedBillboardMatrix;
        DirectX::XMMATRIX m_CachedMainViewProjectionMatrix;
        DirectX::XMMATRIX m_CachedMainInvProjectionMatrix;
        DirectX::XMMATRIX m_CachedMainInvViewMatrix;

    protected:
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;

    public:
        void Present();
        void InitDrawOnBackBufferPipeline();
        void DrawMeshInstanced(ID3D11Buffer* vertexBuffer,
                               ID3D11Buffer* indexBuffer,
                               uint32_t      indexCount,
                               uint32_t      vertexSize,
                               Material*     material,
                               uint32_t      instanceCount,
                               uint32_t      instanceOffset);

        using native_handle_type = void*;
        native_handle_type m_WindowHandle;

        void         SetWindowHandle(native_handle_type handle);
        void         SetMainCameraComponent(ComponentHandle cameraHandle);
        void         RefreshMainCameraSettings();
        inline float GetWindowAspectRatio()
        {
                return m_BackBufferWidth / m_BackBufferHeight;
        };
        void OnWindowResize(unsigned int wParam, unsigned int lParam, bool goFullscreen);
        void SetFullscreen(bool);
        bool GetFullscreen();

        inline IDXGISwapChain1* GetSwapChain()
        {
                return m_Swapchain;
        };
        inline ID3D11Device1* GetDevice()
        {
                return m_Device;
        }
        inline ID3D11DeviceContext1* GetContext()
        {
                return m_Context;
        }
};
