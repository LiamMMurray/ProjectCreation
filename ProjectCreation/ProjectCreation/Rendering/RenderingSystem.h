#pragma once

/** Includes **/

#include "../ECS/ECS.h"
#include "../Utility/ForwardDeclarations/D3DNativeTypes.h"
#include "../Utility/ForwardDeclarations/WinProcTypes.h"

#include "../ResourceManager/IResource.h"

#include "ConstantBuffers.h"

#include <DirectXMath.h>


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
                DIFFUSE_REFLECTION_MAP,
                SPECULAR_REFLECTION_MAP,
                INTEGRATION_MAP,
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
                COUNT
        };
};

struct E_PIXEL_SHADERS
{
        enum
        {
                DEFAULT = 0,
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
                COUNT
        };
};

struct E_RASTERIZER_STATE
{
        enum
        {
                DEFAULT = 0,
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

struct E_STATE_DEPTH_STENCIL
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

/** Class Declarations **/

class RenderSystem : public ISystem
{
        friend class ResourceManager;

        using native_handle_type = void*;
        native_handle_type m_WindowHandle;

        IDXGISwapChain1*      m_Swapchain;
        ID3D11Device1*        m_Device;
        ID3D11DeviceContext1* m_Context;

        void CreateDeviceAndSwapChain();
        void CreateDefaultRenderTargets();
        void CreateRasterizerStates();
        void CreateInputLayouts();
        void CreateCommonShaders();
        void CreateCommonConstantBuffers();
        void CreateSamplerStates();

        ID3D11RenderTargetView*   m_DefaultRenderTargets[E_RENDER_TARGET::COUNT]{};
        ID3D11ShaderResourceView* m_PostProcessSRVs[E_POSTPROCESS_PIXEL_SRV::COUNT]{};
        ID3D11DepthStencilView*   m_DefaultDepthStencil[E_DEPTH_STENCIL::COUNT]{};
        ID3D11InputLayout*        m_DefaultInputLayouts[E_INPUT_LAYOUT::COUNT]{};
        ID3D11RasterizerState*    m_DefaultRasterizerStates[E_RASTERIZER_STATE::COUNT]{};
        ID3D11SamplerState*       m_DefaultSamplerStates[E_SAMPLER_STATE::COUNT]{};

        ResourceHandle m_CommonVertexShaderHandles[E_VERTEX_SHADERS::COUNT];
        ResourceHandle m_CommonPixelShaderHandles[E_PIXEL_SHADERS::COUNT];

        ID3D11Buffer* m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::COUNT];

        CTransformBuffer m_ConstantBuffer_MVP;
        CSceneInfoBuffer m_ConstantBuffer_SCENE;
        CAnimationBuffer m_ConstantBuffer_ANIM;

        float m_BackBufferWidth;
        float m_BackBufferHeight;

        void UpdateConstantBuffer(ID3D11Buffer* gpuBuffer, void* cpuBuffer, size_t size);

        void DrawOpaqueStaticMesh(StaticMesh* mesh, Material* material, DirectX::XMMATRIX* mtx);
        void DrawOpaqueSkeletalMesh(SkeletalMesh*               mesh,
                                    Material*                   material,
                                    DirectX::XMMATRIX*          mtx,
                                    const Animation::FSkeleton* skel);
        void DrawTransparentStaticMesh(StaticMesh* mesh, Material* material, DirectX::XMMATRIX* mtx);
        void DrawTransparentSkeletalMesh(SkeletalMesh* mesh, Material* material, DirectX::XMMATRIX* mtx);


        ComponentHandle   m_MainCameraHandle;
        ComponentHandle   m_MainTransformHandle;
        ResourceManager*  m_ResourceManager;
        ComponentManager* m_ComponentManager;
        EntityManager*    m_EntityManager;

        DirectX::XMMATRIX m_CachedMainProjectionMatrix;

    protected:
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;

    public:
        void         SetWindowHandle(native_handle_type handle);
        void         SetMainCameraComponent(ComponentHandle cameraHandle);
        void         RefreshMainCameraSettings();
        inline float GetWindowAspectRatio()
        {
                return m_BackBufferWidth / m_BackBufferHeight;
        };
        void OnWindowResize(WPARAM wParam, LPARAM lParam);
        void SetFullscreen(bool);
        bool GetFullscreen();
};
