#include <WindowsUtility.h>

#include <UIManager.h>
#include <CoreInput.h>
namespace WindowsUtil
{
        bool    runMessageLoop = false;
        LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
                // switch based on the input message
                if (runMessageLoop)
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
                                case WM_WINDOWPOSCHANGED:
                                {
                                        if (UIManager::instance)
                                        {
                                                UIManager::instance->CatchWinProcSetFullscreen();
                                        }
                                        // GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>()->OnWindowResize(wParam,
                                        // lParam);
                                        break;
                                }
                        }

                // Any other messages, handle the default way
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
        void LaunchMessageLoop()
        {
                runMessageLoop = true;
        }
        HWND CreateDefaultEngineWindow()
        {
                constexpr char appName[] = "Inanis";
                // window info
                WNDCLASSEX winInfo;
                // zero out the win info struct
                ZeroMemory(&winInfo, sizeof(WNDCLASSEX));
                // Set win info struct data
                winInfo.cbSize        = sizeof(WNDCLASSEX);      // sets size of struct. Must because of backwards compatibility
                winInfo.style         = CS_HREDRAW | CS_VREDRAW; // redraws whole window on horizontal or vertical modification
                winInfo.lpfnWndProc   = WindowProc;              // ptr to the window procedure
                winInfo.hInstance     = GetModuleHandle(0);               // ptr to app instance
                winInfo.hCursor       = LoadCursor(nullptr, IDC_ARROW); // set cursor for this window
                winInfo.hbrBackground = (HBRUSH)COLOR_WINDOW;           // brush or color used for background
                winInfo.lpszClassName = appName;                        // Window class name. Local to current process

                // register window class
                RegisterClassEx(&winInfo);
                DWORD style = WS_POPUP;
                if (GEngine::ShowFPS)
                        style = WS_OVERLAPPEDWINDOW;

                RECT wr = {0, 0, 1600, 900};         // set the size
                AdjustWindowRect(&wr, style, FALSE); // adjust the size

                int posX = GetSystemMetrics(SM_CXSCREEN) / 2 - (wr.right - wr.left) / 2;
                int posY = GetSystemMetrics(SM_CYSCREEN) / 2 - (wr.bottom - wr.top) / 2;

                // create the window and use the result as the handle
                HWND handle = CreateWindowEx(WS_EX_APPWINDOW,
                                             appName, // Window class name again
                                             appName, // window title text
                                             style,
                                             posX,                // x pos
                                             posY,                // y pos
                                             wr.right - wr.left,  // width of the window
                                             wr.bottom - wr.top,  // height of the window
                                             nullptr,             // parent handle
                                             nullptr,             // menu HMENU?
                                             GetModuleHandleW(0), // app instance
                                             nullptr);            // parameters passed to new window (32 bit value)
                return handle;
        }
} // namespace WindowsUtil