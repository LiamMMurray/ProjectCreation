#include <StaticSentinelDumpMemoryLeaks.h> 

#include <stdio.h>
#include <time.h>
#include <windowsx.h>

#include <DbgHelp.h>
#pragma comment(lib, "dbghelp")

#include <Interface/G_Audio/GMusic.h>
#include <Interface/G_Audio/GSound.h>

#include <ContinousSoundSystem.h>

#include <UIManager.h>

#include <CollisionLibary.h>
#include <CollisionResult.h>

#include <CoreInput.h>

#include <ConsoleWindow.h>

#include<EngineInitShutdownHelpers.h>
#include <EntityFactory.h>

#include <AnimationSystem.h>
#include <EmitterComponent.h>
#include <ParticleData.h>
#include <SkeletalMesh.h>
#include <CameraComponent.h>
#include <SkeletalMeshComponent.h>
#include <StaticMeshComponent.h>
/////testing -vic
#include <debug_renderer.h>
////testing -vic
#include <LevelStateManager.h>
#include <ControllerSystem.h>
#include <OrbitSystem.h>
#include <SpeedBoostSystem.h>
#include <DirectionalLightComponent.h>


using namespace DirectX;
////testing -vic
using namespace Shapes;
using namespace Collision;
using namespace debug_renderer;
////testing -vic

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

#include <WindowsUtility.h> 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
        std::srand(unsigned(std::time(0)));
        JGamePad::Get()->Init();


        // console window creation
        ConsoleWindow CW;
        CW.CreateConsoleWindow("Inanis Console Window");

		HWND mainWindowHandle = WindowsUtil::CreateDefaultEngineWindow();

        /** Main init engine **/
        EngineUtil::InitEngineSystemManagers(mainWindowHandle);
        SystemManager*   systemManager   = GEngine::Get()->GetSystemManager();
        HandleManager*   HandleManager   = GEngine::Get()->GetHandleManager();

        // message loop
        ShowWindow(mainWindowHandle, SW_SHOW);
        WindowsUtil::LaunchMessageLoop();

        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        PlayerController pMovement;

        // Joseph found an underwater abience music track and is replacing the old music with it for testing <06/19/19>
        // auto music = AudioManager::Get()->LoadMusic("extreme");
        auto music = AudioManager::Get()->LoadMusic("Ambience 16-48k");
        AudioManager::Get()->ActivateMusicAndPause(music, true);
        music->ResumeStream();

        // Directional Light setup
        {
                using namespace DirectX;

                auto dirLightEntityHandle = HandleManager->CreateEntity();
                HandleManager->AddComponent<DirectionalLightComponent>(dirLightEntityHandle);
                HandleManager->AddComponent<TransformComponent>(dirLightEntityHandle);
                EmitterComponent* emitterComponent =
                    HandleManager->AddComponent<EmitterComponent>(dirLightEntityHandle).Get<EmitterComponent>();

                // emitter set up
                XMFLOAT3 position;
                XMStoreFloat3(&position, dirLightEntityHandle.GetComponent<TransformComponent>()->transform.translation);
                emitterComponent->FloatParticle(XMFLOAT3(-30.0f, -5.0f, -30.0f),
                                                XMFLOAT3(30.0f, 10.0f, 30.0f),
                                                XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
                                                XMFLOAT4(1.0f, 1.0f, 1.0f, 0.65f),
                                                XMFLOAT4(8.0f, 3.0f, 1.0f, 1.0f));
                emitterComponent->EmitterData.emitterPosition    = position;
                emitterComponent->rotate                         = false;
                emitterComponent->maxCount                       = ParticleData::gMaxParticleCount;
                emitterComponent->spawnRate                      = 10000.0f;
                emitterComponent->EmitterData.textureIndex       = 3;
                emitterComponent->EmitterData.minInitialVelocity = {-1.05f, -0.4f, -1.05f};
                emitterComponent->EmitterData.maxInitialVelocity = {1.05f, 0.05f, 1.05f};
                emitterComponent->EmitterData.particleScale      = {0.2f, 0.2f};
                emitterComponent->desiredCount                   = ParticleData::gMaxEmitterCount;


                auto dirComp = dirLightEntityHandle.GetComponent<DirectionalLightComponent>();
                dirComp->m_LightRotation =
                    XMQuaternionRotationRollPitchYaw(XMConvertToRadians(25.0f), XMConvertToRadians(90.0f), 0.0f);
                dirComp->m_LightColor   = XMFLOAT4(1.0f, 0.85f, 0.7f, 5.0f);
                dirComp->m_AmbientColor = XMFLOAT4(1.0f, 0.85f, 0.7f, 1.7f);


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


        GEngine::Get()->SetGamePaused(true);
        GEngine::Get()->GetLevelStateManager()->Init();
        UIManager::instance->StartupResAdjust(mainWindowHandle);


        GEngine::Get()->Signal();
        while (msg.message != WM_QUIT && !GEngine::Get()->WantsGameExit())
        {
                GCoreInput::UpdateInput();

                GEngine::Get()->m_MainThreadProfilingContext.Begin("Main Loop", "PeekMessage");
                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                        if (msg.message == WM_QUIT)
                                break;
                }
                GEngine::Get()->m_MainThreadProfilingContext.End();

                GEngine::Get()->m_MainThreadProfilingContext.Begin("Main Loop", "GEngine::Signal");
                // Main application loop goes here.
                float deltaTime = GEngine::Get()->Update();
                GEngine::Get()->m_MainThreadProfilingContext.End();

                GEngine::Get()->m_MainThreadProfilingContext.Begin("Main Loop", "Other");
                if (GetActiveWindow() != mainWindowHandle && GEngine::Get()->GetGamePaused() == false)
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
                                SetWindowTextA(mainWindowHandle, buffer);
                                framesPast = frameCount;
                                prevCount  = GetTickCount();
                        }
                }


                debug_renderer::AddGrid(XMVectorZero(), 10.0f, 10, ColorConstants::White);
                GEngine::Get()->GetSystemManager()->Update(deltaTime);
                GEngine::Get()->m_MainThreadProfilingContext.End();
        }
        JGamePad::Shutdown();
        EngineUtil::ShutdownEngineSystemManagers();

        return 0;
}