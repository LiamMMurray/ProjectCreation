#include "Utility/StaticSentinelDumpMemoryLeaks.h"

#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#define NOMINMAX
#include <Windows.h>

#include <stdio.h>
#include <time.h>
#include <windowsx.h>

#include <DbgHelp.h>

#include <Interface/G_Audio/GMusic.h>
#include <Interface/G_Audio/GSound.h>

#include "UI/UIManager.h"

#include "Engine/CollisionLibary/CollisionLibary.h"
#include "Engine/CollisionLibary/CollisionResult.h"

#include "Engine/CoreInput/CoreInput.h"

#include "Engine/ConsoleWindow/ConsoleWindow.h"

#include "Engine/EngineInitShutdownHelpers.h"
#include "Engine/Entities/EntityFactory.h"

#include "Rendering/Components/CameraComponent.h"
#include "Rendering/Components/SkeletalMeshComponent.h"
#include "Rendering/Components/StaticMeshComponent.h"

#include "Engine/Animation/AnimationSystem.h"
#include "Engine/ResourceManager/SkeletalMesh.h"
/////testing -vic
#include "Rendering/DebugRender/debug_renderer.h"
////testing -vic
///

#include "Engine/Levels/LevelStateManager.h"

#include "Engine/Controller/ControllerSystem.h"

#include "Engine/Gameplay/OrbitSystem.h"
#include "Engine/Gameplay/SpeedBoostSystem.h"
#include "Rendering/Components/DirectionalLightComponent.h"

#include "Engine/AI/AISystem.h"
#include "Engine/GenericComponents/TransformComponent.h"
#include "Engine/MathLibrary/MathLibrary.h"
#include "Utility/MemoryLeakDetection.h"

#pragma comment(lib, "dbghelp")

using namespace DirectX;
////testing -vic
using namespace Shapes;
using namespace Collision;
using namespace debug_renderer;
////testing -vic
bool g_Running = false;

LONG WINAPI errorFunc(_EXCEPTION_POINTERS* pExceptionInfo)
{
        /*
            This will give you a date/time formatted string for your dump files
            Make sure to include these files:
            #include <DbgHelp.h>
            #include <stdio.h>
            #include <time.h>

            AND this lib:
            dbghelp.lib
        */
        struct tm newTime;
        time_t    ltime;
        wchar_t   buff[100] = {0};

        ltime = time(&ltime);
        localtime_s(&newTime, &ltime);

        wcsftime(buff, sizeof(buff), L"%A_%b%d_%I%M%p.mdmp", &newTime);

        HANDLE hFile = ::CreateFileW(
            /*L"dumpfile.mdmp"*/ buff, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hFile != INVALID_HANDLE_VALUE)
        {
                _MINIDUMP_EXCEPTION_INFORMATION ExInfo;

                ExInfo.ThreadId          = ::GetCurrentThreadId();
                ExInfo.ExceptionPointers = pExceptionInfo;
                ExInfo.ClientPointers    = NULL;
                MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);

                // MessageBox("Dump File Saved look x directory please email to developer at the following email adress
                // crashdmp@gmail.com with the subject Gamename - Version ");
                ::CloseHandle(hFile);
        }

        return 0;
}

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
                                GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>()->OnWindowResize(wParam, lParam);
                                break;
                        }
                }

        // Any other messages, handle the default way
        return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI _WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

// Windows version of main. WINAPI reverses order of parameters
int WINAPI WinMain(HINSTANCE hInstance,     // ptr to current instance of app
                   HINSTANCE hPrevInstance, // prev instance of app. Is always null nowadays.
                   LPSTR     lpCmdLine,     // command line of app excluding program name
                   int       nCmdShow       // how the windows is shown. Legacy. Can ignore
)
{
        ENABLE_LEAK_DETECTION();

        _WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

        return 0;
}


int WINAPI _WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

        std::srand(unsigned(std::time(0)));

        constexpr char appName[] = "Inanis";
        // window info
        WNDCLASSEX winInfo;

        // console window creation
        ConsoleWindow CW;
        CW.CreateConsoleWindow("Inanis Console Window");

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

        DWORD style = WS_POPUP;
        if (GEngine::ShowFPS)
                style = WS_OVERLAPPEDWINDOW;

        RECT wr = {0, 0, 1920, 1080};        // set the size
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


        /** Main init engine **/
        EngineHelpers::InitEngineSystemManagers(handle);
        SystemManager*   systemManager   = GEngine::Get()->GetSystemManager();
        HandleManager*   HandleManager   = GEngine::Get()->GetHandleManager();
        ResourceManager* resourceManager = GEngine::Get()->GetResourceManager();

        // message loop
        ShowWindow(handle, SW_SHOW);
        g_Running = true;
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        PlayerController pMovement;


        // Sound tests

        // Joseph found a sfx track for speed boosts and is replacing the old music with it for testing <06/19/19>
        auto boop = AudioManager::Get()->CreateSFX("whiteSpeedBoost");
        boop->SetVolume(0.1f);

        // Joseph found an underwater abience music track and is replacing the old music with it for testing <06/19/19>
        // auto music = AudioManager::Get()->LoadMusic("extreme");

        auto music = AudioManager::Get()->LoadMusic("Ambience 16-48k");
        AudioManager::Get()->ActivateMusicAndPause(music, true);

        music->ResumeStream();

        // Entity tests

        // auto testMeshHandle = HandleManager->CreateEntity();
        // auto entity  = HandleManager->GetEntity(eHandle);

        // Debug camera entity setup

        // Test skeletal mesh setup
        {
            /*std::vector<std::string> animNames = {"Idle", "Walk", "Run"};
            ComponentHandle          transformHandle;
            EntityFactory::CreateSkeletalMeshEntity("Walk", "NewMaterial", animNames, nullptr, &transformHandle);
           







            TransformComponent* transformComp = HandleManager->GetComponent<TransformComponent>(transformHandle);
            transformComp->transform.SetScale(0.1f);*/
        }

        // Ground Plane
        {
            // ComponentHandle tHandle;
            // EntityFactory::CreateStaticMeshEntity("GroundPlane01", "GroundMaterial01", &tHandle, nullptr, false);
            // TransformComponent* tComp    = tHandle.Get<TransformComponent>();
            // tComp->transform.translation = XMVectorSetY(tComp->transform.translation, -0.0f);
        }

        // Directional Light setup
        {
                using namespace DirectX;

                auto dirLightEntityHandle = HandleManager->CreateEntity();
                HandleManager->AddComponent<DirectionalLightComponent>(dirLightEntityHandle);

                auto dirComp = dirLightEntityHandle.GetComponent<DirectionalLightComponent>();
                dirComp->m_LightRotation =
                    XMQuaternionRotationRollPitchYaw(XMConvertToRadians(25.0f), XMConvertToRadians(90.0f), 0.0f);
                dirComp->m_LightColor   = XMFLOAT4(1.0f, 0.85f, 0.7f, 8.0f);
                dirComp->m_AmbientColor = XMFLOAT4(1.0f, 0.85f, 0.7f, 1.1f);

                GEngine::Get()->m_SunHandle = dirLightEntityHandle;
        }

        // Create speedboost system
        {
                FSystemProperties sysInitProps;

                sysInitProps.m_Priority   = E_SYSTEM_PRIORITY::NORMAL;
                sysInitProps.m_UpdateRate = 0.0f;

                OrbitSystem* orbitSystem;
                systemManager->CreateSystem<OrbitSystem>(&orbitSystem);
                systemManager->RegisterSystem(&sysInitProps, orbitSystem);
                orbitSystem->m_SystemName = "OrbitSystem";

                SpeedBoostSystem* speedBoostSystem;
                systemManager->CreateSystem<SpeedBoostSystem>(&speedBoostSystem);
                systemManager->RegisterSystem(&sysInitProps, speedBoostSystem);
                speedBoostSystem->m_SystemName = "SpeedBoostSystem";
        }

        // Create AI System
        {
                FSystemProperties sysInitProps;
                sysInitProps.m_Priority   = E_SYSTEM_PRIORITY::NORMAL;
                sysInitProps.m_UpdateRate = 0.0f;

                AISystem* aiSystem;
                systemManager->CreateSystem<AISystem>(&aiSystem);
                systemManager->RegisterSystem(&sysInitProps, aiSystem);
                aiSystem->m_SystemName = "AISystem";
        }


        GEngine::Get()->SetGamePaused(true);
        GEngine::Get()->GetLevelStateManager()->Init();
        UIManager::instance->StartupResAdjust(handle);

        while (msg.message != WM_QUIT && !GEngine::Get()->WantsGameExit())
        {
                GCoreInput::UpdateInput();

                GEngine::Get()->m_MainThreadProfilingContext.Begin("Main Loop", "PeekMessage");
                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                        // translate keystroke messages into the right format
                        TranslateMessage(&msg);

                        // send the message to the WindowProc function
                        DispatchMessage(&msg);

                        // check to see if it's time to quit
                        if (msg.message == WM_QUIT)
                                break;
                }
                GEngine::Get()->m_MainThreadProfilingContext.End();

                GEngine::Get()->m_MainThreadProfilingContext.Begin("Main Loop", "GEngine::Signal");
                // Main application loop goes here.
                float deltaTime = GEngine::Get()->Update();
                GEngine::Get()->m_MainThreadProfilingContext.End();

                GEngine::Get()->m_MainThreadProfilingContext.Begin("Main Loop", "Other");
                if (GetActiveWindow() != handle && GEngine::Get()->GetGamePaused() == false)
                {
                        UIManager::instance->Pause();
                }


                {
                        static DWORD frameCount = 0;
                        ++frameCount;
                        static DWORD framesPast = frameCount;
                        static DWORD prevCount  = (DWORD)GEngine::Get()->GetTotalTime();
                        if (GetTickCount() - prevCount > 1000) // only update every second
                        {
                                char buffer[256];
                                sprintf_s(buffer, "DirectX Test. FPS: %d", frameCount - framesPast);
                                SetWindowTextA(static_cast<HWND>(handle), buffer);
                                framesPast = frameCount;
                                prevCount  = GetTickCount();
                        }
                }

                if (GCoreInput::GetKeyState(KeyCode::P) == KeyState::DownFirst)
                {
                        boop->Play();
                }
                if (GCoreInput::GetKeyState(KeyCode::O) == KeyState::DownFirst)
                {
                        bool musicIsPlaying;
                        music->isStreamPlaying(musicIsPlaying);
                        if (!musicIsPlaying)
                                music->ResumeStream();
                        else
                                music->PauseStream();
                }

                debug_renderer::AddGrid(XMVectorZero(), 10.0f, 10, ColorConstants::White);
                GEngine::Get()->GetSystemManager()->Update(deltaTime);
                GEngine::Get()->m_MainThreadProfilingContext.End();
        }
        EngineHelpers::ShutdownEngineSystemManagers();

        return 0;
}
