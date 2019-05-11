#pragma once

#include "../ECS/ECS.h"
#include "../Utility/ForwardDeclarations/WinProcTypes.h"
#include "../Utility/ForwardDeclarations/D3DNativeTypes.h"

struct EVIEWPORT
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct ECONSTANT_BUFFER
{
        enum
        {
                MVP = 0,
                COUNT
        };
};

struct EVERTEX_SHADER_RESOURCE_VIEW
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct EPIXEL_SHADER_RESOURCE_VIEW
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

struct ERENDER_TARGET
{
        enum
        {
                BACKBUFFER = 0,
                COUNT
        };
};

struct EINPUT_LAYOUT
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct ESTATE_RASTERIZER
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct EVIEW_DEPTH_STENCIL
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct ESTATE_DEPTH_STENCIL
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

class CRenderSystem : public ISystem
{
        using native_handle_type = void*;
        native_handle_type m_WindowHandle;

        IDXGISwapChain1*      m_Swapchain;
        ID3D11Device1*        m_Device;
        ID3D11DeviceContext1* m_Context;

        void CreateDeviceAndSwapChain();
        void CreateBackbufferRenderTarget();
        void CreateRasterizerStates();

        ID3D11RenderTargetView* m_RenderTargets[ERENDER_TARGET::COUNT]{};

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

