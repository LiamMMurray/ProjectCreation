#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#include <Windows.h>
#include <windowsx.h>

#include "CoreInput/CoreInput.h"
#include "Engine/GEngine.h"

#include "Rendering/RenderingSystem.h"

bool g_Running = false;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
        // switch based on the input message
        if (g_Running)
        switch (message)
        {
                // We are told to close the app
                case WM_DESTROY:
                {
                        PostQuitMessage(0);
                        return 0;
                }
                case WM_INPUT:
                {
                        GCoreInput::GatherInput(hWnd, message, wParam, lParam);
                        break;
                }
                case WM_SIZE:
                {
                        GEngine::Get()->GetSystemManager()->GetSystem<CRenderSystem>()->OnWindowResize(wParam, lParam);
                        break;
                }
        }

        // Any other messages, handle the default way
        return DefWindowProc(hWnd, message, wParam, lParam);
}


// Windows version of main. WINAPI reverses order of parameters
int WINAPI WinMain(HINSTANCE hInstance,     // ptr to current instance of app
                   HINSTANCE hPrevInstance, // prev instance of app. Is always null nowadays.
                   LPSTR     lpCmdLine,     // command line of app excluding program name
                   int       nCmdShow       // how the windows is shown. Legacy. Can ignore
)
{
        const char* appName = "Inanis";
        // window info
        WNDCLASSEX winInfo;

        // zero out the win info struct
        ZeroMemory(&winInfo, sizeof(WNDCLASSEX));

        // Set win info struct data
        winInfo.cbSize        = sizeof(WNDCLASSEX);             // sets size of struct. Must because of backwards compatibility
        winInfo.style         = CS_HREDRAW | CS_VREDRAW;        // redraws whole window on horizontal or vertical modification
        winInfo.lpfnWndProc   = WindowProc;                     // ptr to the window procedure
        winInfo.hInstance     = hInstance;                      // ptr to app instance
        winInfo.hCursor       = LoadCursor(nullptr, IDC_ARROW); // set cursor for this window
        winInfo.hbrBackground = (HBRUSH)COLOR_WINDOW;           // brush or color used for background
        winInfo.lpszClassName = appName;                        // Window class name. Local to current process

        // register window class
        RegisterClassEx(&winInfo);

        RECT wr = {0, 0, 1280, 720};                       // set the size
        AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); // adjust the size

        int posX = GetSystemMetrics(SM_CXSCREEN) / 2 - (wr.right - wr.left) / 2;
        int posY = GetSystemMetrics(SM_CYSCREEN) / 2 - (wr.bottom - wr.top) / 2;

        // create the window and use the result as the handle
        HWND handle = CreateWindowEx(WS_EX_APPWINDOW,
                                     appName, // Window class name again
                                     appName, // window title text
                                     WS_OVERLAPPED | WS_SYSMENU,
                                     posX,                // x pos
                                     posY,                // y pos
                                     wr.right - wr.left,  // width of the window
                                     wr.bottom - wr.top,  // height of the window
                                     nullptr,             // parent handle
                                     nullptr,             // menu HMENU?
                                     GetModuleHandleW(0), // app instance
                                     nullptr);            // parameters passed to new window (32 bit value)


        GEngine::Initialize();

        SystemManager*    systemManager    = GEngine::Get()->GetSystemManager();
        EntityManager*    entityManager    = GEngine::Get()->GetEntityManager();
        ComponentManager* componentManager = GEngine::Get()->GetComponentManager();

        CRenderSystem* renderSystem;
        systemManager->CreateSystem<CRenderSystem>(&renderSystem);
		FSystemInitProperties sysInitProps;
        renderSystem->SetWindowHandle(handle);
        systemManager->RegisterSystem(&sysInitProps, renderSystem);

        GCoreInput::InitializeInput(handle);
        // message loop
        ShowWindow(handle, SW_SHOW);
        g_Running = true;
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        while (msg.message != WM_QUIT)
        {
                GCoreInput::UpdateInput();

                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                        // translate keystroke messages into the right format
                        TranslateMessage(&msg);

                        // send the message to the WindowProc function
                        DispatchMessage(&msg);

                        // check to see if it's time to quit
                        if (msg.message == WM_QUIT)
                                return false;
                }

                // Main application loop goes here.
                GEngine::Get()->Signal();

                GEngine::Get()->GetSystemManager()->Update(GEngine::Get()->GetDeltaTime());
        }


        GEngine::Shutdown();

        return 0;
}