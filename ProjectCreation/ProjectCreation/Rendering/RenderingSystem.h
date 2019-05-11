#pragma once

#include "../ECS/ECS.h"

typedef void* WindowHandle;

struct IDXGISwapChain1;
struct ID3D11Device1;
struct ID3D11DeviceContext1;


class CRenderSystem : public ISystem
{
        WindowHandle m_WindowHandle;

        IDXGISwapChain1*      m_Swapchain;
        ID3D11Device1*        m_Device;  
        ID3D11DeviceContext1* m_Context;

		void CreateDeviceAndSwapChain();
        void CreateBackbuffer();
        void CreateRasterizerStates();

    protected:
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;

    public:
        void SetWindowHandle(WindowHandle* handle);
};

struct VIEWPORT
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct CONSTANT_BUFFER
{
        enum
        {
                MVP = 0,
                COUNT
        };
};

struct VERTEX_SHADER_RESOURCE_VIEW
{
        enum
        {
                COLORED_VERTEX = 0,
                CUBE,
                COUNT
        };
};

struct PIXEL_SHADER_RESOURCE_VIEW
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

struct VIEW_RENDER_TARGET
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct INPUT_LAYOUT
{
        enum
        {
                COLORED_VERTEX = 0,
                COUNT
        };
};

struct STATE_RASTERIZER
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct VIEW_DEPTH_STENCIL
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct STATE_DEPTH_STENCIL
{
        enum
        {
                DEFAULT = 0,
                COUNT
        };
};

struct VERTEX_BUFFER
{
        enum
        {
                COLORED_VERTEX = 0,
                TERRAIN        = 1,
                COUNT          = 2
        };
};