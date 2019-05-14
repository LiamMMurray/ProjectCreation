#pragma once

#include "../ECS/ECS.h"
#include "../Utility/ForwardDeclarations/D3DNativeTypes.h"
#include "../Utility/ForwardDeclarations/WinProcTypes.h"

struct CRenderComponent;

struct E_VIEWPORT
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct E_CONSTANT_BUFFER
{
        enum
        {
                MVP = 0,
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

struct E_POSTPROCESS_PIXEL_SRV
{
        enum
        {
                BASE_PASS = 0,
                BASE_DEPTH,


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

class CRenderSystem : public ISystem
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


        ID3D11RenderTargetView*   m_DefaultRenderTargets[E_RENDER_TARGET::COUNT]{};
        ID3D11ShaderResourceView* m_PostProcessSRVs[E_POSTPROCESS_PIXEL_SRV::COUNT]{};
        ID3D11DepthStencilView*   m_DefaultDepthStencil[E_DEPTH_STENCIL::COUNT]{};


    protected:
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;

    public:
        void SetWindowHandle(native_handle_type handle);

        void OnWindowResize(WPARAM wParam, LPARAM lParam);
        void SetFullscreen(bool);
        bool GetFullscreen();
};
