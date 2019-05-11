#include "RenderingSystem.h"

#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")

#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#include <Windows.h>
#include <windowsx.h>

void CRenderSystem::OnPreUpdate(float deltaTime)
{}

void CRenderSystem::OnUpdate(float deltaTime)
{}

void CRenderSystem::OnPostUpdate(float deltaTime)
{}

void CRenderSystem::OnInitialize()
{
        assert(m_WindowHandle);

		RECT rect;
        bool bWindowRect = GetWindowRect((HWND)m_WindowHandle, &rect);
        assert(bWindowRect);


}

void CRenderSystem::OnShutdown()
{
        m_Swapchain->Release();
        m_Context->Release();
        m_Device->Release();
}

void CRenderSystem::OnResume()
{}

void CRenderSystem::OnSuspend()
{}

void CRenderSystem::SetWindowHandle(WindowHandle* handle)
{
        m_WindowHandle = handle;
}
