
#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#define NOMINMAX
#include <Windows.h>

#include <stdio.h>
#include <time.h>
#include <windowsx.h>

#include <DbgHelp.h>

#include <Interface/G_Audio/GMusic.h>
#include <Interface/G_Audio/GSound.h>

#include "Engine/CollisionLibary/CollisionLibary.h"
#include "Engine/CollisionLibary/CollisionResult.h"

#include "Engine/CoreInput/CoreInput.h"

#include "Engine/ConsoleWindow/ConsoleWindow.h"

#include "Engine/EngineInitShutdownHelpers.h"
#include "Engine/Entities/BaseEntities.h"
#include "Engine/Entities/EntityFactory.h"

#include "Rendering/Components/CameraComponent.h"
#include "Rendering/Components/SkeletalMeshComponent.h"
#include "Rendering/Components/StaticMeshComponent.h"

#include "Engine/Animation/AnimationSystem.h"
#include "Engine/ResourceManager/SkeletalMesh.h"
/////testing -vic
#include "Engine/CollisionLibary/CollisionSystem.h"
#include "Rendering/DebugRender/debug_renderer.h"
////testing -vic

#include "Engine/Controller/ControllerSystem.h"

#include "Engine/Gameplay/OrbitSystem.h"
#include "Engine/Gameplay/SpeedBoostSystem.h"
#include "Rendering/Components/DirectionalLightComponent.h"

#include "Engine/GenericComponents/TransformComponent.h"

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


// Windows version of main. WINAPI reverses order of parameters
int WINAPI WinMain(HINSTANCE hInstance,     // ptr to current instance of app
                   HINSTANCE hPrevInstance, // prev instance of app. Is always null nowadays.
                   LPSTR     lpCmdLine,     // command line of app excluding program name
                   int       nCmdShow       // how the windows is shown. Legacy. Can ignore
)
{
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


        /** Main init engine **/
        EngineHelpers::InitEngineSystemManagers(handle);
        SystemManager*    systemManager    = GEngine::Get()->GetSystemManager();
        EntityManager*    entityManager    = GEngine::Get()->GetEntityManager();
        ComponentManager* componentManager = GEngine::Get()->GetComponentManager();
        ResourceManager*  resourceManager  = GEngine::Get()->GetResourceManager();

        // message loop
        ShowWindow(handle, SW_SHOW);
        g_Running = true;
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        PlayerController pMovement;


        // Sound tests
        auto boop  = AudioManager::Get()->CreateSFX("boop");
        auto music = AudioManager::Get()->LoadMusic("extreme");
        AudioManager::Get()->ActivateMusicAndPause(music, true);

        // Entity tests

        auto testMeshHandle = entityManager->CreateEntity<BaseEntity>();
        // auto entity  = entityManager->GetEntity(eHandle);

        // Debug camera entity setup

        // Test skeletal mesh setup
        {
            // std::vector<std::string> animNames = {"Idle", "Walk", "Run"};
            // ComponentHandle          transformHandle;
            // EntityFactory::CreateSkeletalMeshEntity("Walk", "NewMaterial", animNames, nullptr, &transformHandle);
            //
            // TransformComponent* transformComp = componentManager->GetComponent<TransformComponent>(transformHandle);
            // transformComp->transform.SetScale(0.1f);
        }

        // Ground Plane
        {
                ComponentHandle tHandle;
                EntityFactory::CreateStaticMeshEntity("GroundPlane01", "GroundMaterial01", &tHandle);
                TransformComponent* tComp    = componentManager->GetComponent<TransformComponent>(tHandle);
                tComp->transform.translation = XMVectorSetY(tComp->transform.translation, -0.5f);
        }

        // Directional Light setup
        {
                using namespace DirectX;

                auto dirLightEntityHandle = entityManager->CreateEntity<BaseEntity>();
                componentManager->AddComponent<DirectionalLightComponent>(dirLightEntityHandle);

                auto dirComp = componentManager->GetComponent<DirectionalLightComponent>(dirLightEntityHandle);
                dirComp->m_LightRotation =
                    XMQuaternionRotationRollPitchYaw(XMConvertToRadians(45.0f), XMConvertToRadians(120.0f), 0.0f);
                dirComp->m_LightColor   = XMFLOAT4(1.0f, 0.8f, 1.0f, 1.0f);
                dirComp->m_AmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.2f);
        }

        // Create speedboost system
        FSystemProperties sysInitProps;
        SpeedBoostSystem*     speedBoostSystem;
        systemManager->CreateSystem<SpeedBoostSystem>(&speedBoostSystem);
        sysInitProps.m_Priority   = E_SYSTEM_PRIORITY::NORMAL;
        sysInitProps.m_UpdateRate = 0.0f;
        systemManager->RegisterSystem(&sysInitProps, speedBoostSystem);

        OrbitSystem* orbitSystem;
        systemManager->CreateSystem<OrbitSystem>(&orbitSystem);
        systemManager->RegisterSystem(&sysInitProps, orbitSystem);
        GEngine::Get()->SetGamePaused(true);
        while (msg.message != WM_QUIT && !GEngine::Get()->WantsGameExit())
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
                                break;
                }

                // Main application loop goes here.
                GEngine::Get()->Signal();

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
                GEngine::Get()->GetSystemManager()->Update(GEngine::Get()->GetDeltaTime());
        }
        EngineHelpers::ShutdownEngineSystemManagers();

        return 0;
}
