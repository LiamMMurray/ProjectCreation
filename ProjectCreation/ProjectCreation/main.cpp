//#include "Utility/StaticSentinelDumpMemoryLeaks.h"
//
//#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
//#define NOMINMAX
//#include <Windows.h>
//
//#include <stdio.h>
//#include <time.h>
//#include <windowsx.h>
//
//#include <DbgHelp.h>
//
//#include <Interface/G_Audio/GMusic.h>
//#include <Interface/G_Audio/GSound.h>
//
//#include "UI/UIManager.h"
//
//#include "Engine/CollisionLibary/CollisionLibary.h"
//#include "Engine/CollisionLibary/CollisionResult.h"
//
//#include "Engine/CoreInput/CoreInput.h"
//
//#include "Engine/ConsoleWindow/ConsoleWindow.h"
//
//#include "Engine/EngineInitShutdownHelpers.h"
//#include "Engine/Entities/EntityFactory.h"
//
//#include "Rendering/Components/CameraComponent.h"
//#include "Rendering/Components/SkeletalMeshComponent.h"
//#include "Rendering/Components/StaticMeshComponent.h"
//
//#include "Engine/Animation/AnimationSystem.h"
//#include "Engine/ResourceManager/SkeletalMesh.h"
///////testing -vic
//#include "Rendering/DebugRender/debug_renderer.h"
//////testing -vic
/////
//
//#include "Engine/Levels/LevelStateManager.h"
//
//#include "Engine/Controller/ControllerSystem.h"
//
//#include "Engine/Gameplay/OrbitSystem.h"
//#include "Engine/Gameplay/SpeedBoostSystem.h"
//#include "Rendering/Components/DirectionalLightComponent.h"
//
//#include "Engine/AI/AISystem.h"
//#include "Engine/GenericComponents/TransformComponent.h"
//#include "Engine/MathLibrary/MathLibrary.h"
//#include "Utility/MemoryLeakDetection.h"
//
//#pragma comment(lib, "dbghelp")
//
// using namespace DirectX;
//////testing -vic
// using namespace Shapes;
// using namespace Collision;
// using namespace debug_renderer;
//////testing -vic
// bool g_Running = false;
//
// LONG WINAPI errorFunc(_EXCEPTION_POINTERS* pExceptionInfo)
//{
//        /*
//            This will give you a date/time formatted string for your dump files
//            Make sure to include these files:
//            #include <DbgHelp.h>
//            #include <stdio.h>
//            #include <time.h>
//
//            AND this lib:
//            dbghelp.lib
//        */
//        struct tm newTime;
//        time_t    ltime;
//        wchar_t   buff[100] = {0};
//
//        ltime = time(&ltime);
//        localtime_s(&newTime, &ltime);
//
//        wcsftime(buff, sizeof(buff), L"%A_%b%d_%I%M%p.mdmp", &newTime);
//
//        HANDLE hFile = ::CreateFileW(
//            /*L"dumpfile.mdmp"*/ buff, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//
//        if (hFile != INVALID_HANDLE_VALUE)
//        {
//                _MINIDUMP_EXCEPTION_INFORMATION ExInfo;
//
//                ExInfo.ThreadId          = ::GetCurrentThreadId();
//                ExInfo.ExceptionPointers = pExceptionInfo;
//                ExInfo.ClientPointers    = NULL;
//                MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
//
//                // MessageBox("Dump File Saved look x directory please email to developer at the following email adress
//                // crashdmp@gmail.com with the subject Gamename - Version ");
//                ::CloseHandle(hFile);
//        }
//
//        return 0;
//}
//
// LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//        // switch based on the input message
//        if (g_Running)
//                switch (message)
//                {
//                        // We are told to close the app
//                        case WM_DESTROY:
//                        {
//                                PostQuitMessage(0);
//                                return 0;
//                        }
//                        case WM_INPUT:
//                        {
//                                GCoreInput::GatherInput(hWnd, message, wParam, lParam);
//                                break;
//                        }
//                        case WM_SIZE:
//                        {
//                                GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>()->OnWindowResize(wParam, lParam);
//                                break;
//                        }
//                }
//
//        // Any other messages, handle the default way
//        return DefWindowProc(hWnd, message, wParam, lParam);
//}
////
//#include<Windows.h>
// int WINAPI _WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
//
//// Windows version of main. WINAPI reverses order of parameters
// int WINAPI WinMain(HINSTANCE hInstance,     // ptr to current instance of app
//                   HINSTANCE hPrevInstance, // prev instance of app. Is always null nowadays.
//                   LPSTR     lpCmdLine,     // command line of app excluding program name
//                   int       nCmdShow       // how the windows is shown. Legacy. Can ignore
//)
//{/*
//        ENABLE_LEAK_DETECTION();*/
//
//        _WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
//
//        return 0;
//}
//#include "Engine/JobScheduler.h"
// int WINAPI _WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//{
//        JobScheduler::Initialize();
//  //
//  //      NMemory::NPools::RandomAccessPools componentRandomAccessPools;
//  //      NMemory::NPools::RandomAccessPools entityRandomAccessPools;
//  //      NMemory::MemoryStack               pool_memory;
//  //      NMemory::byte*                     vmem = static_cast<NMemory::byte*>(malloc(KB(128)));
//  //      NMemory::ReserveGameMemory(pool_memory, MB(128));
//  //      HandleManager HandleManager(componentRandomAccessPools, entityRandomAccessPools, pool_memory);
//
//  //      constexpr unsigned        NumEntities = 1500;
//  //      std::vector<EntityHandle> Entities;
//  //      for (unsigned i = 0; i < NumEntities; i++) {
//  //              Entities.push_back(HandleManager.CreateEntity());
//		//}
//  //      for (auto& itr : Entities)
//  //              itr.AddComponent<TransformComponent>();
//  //      auto& debug        = JobSchedulerInternal::TempJobAllocatorImpl;
//  //      auto [data, count] = HandleManager.GetComponentsRaw<TransformComponent>();
//  //      auto initScaleJob = ParallelFor([&data](unsigned index) { data[index].transform.scale = {1, 1, 1}; }, 0, count);
//  //      initScaleJob();
//  //      initScaleJob.Wait();
//        auto& i = JobSchedulerInternal::TempJobAllocatorImpl;
//        auto& j = JobSchedulerInternal::StaticJobAllocatorImpl;
//
//		JobScheduler::Shutdown();
//        //HandleManager.Shutdown();
//        return 0;
//}
#include "Engine/JobScheduler.h"
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
        auto addr3 = &JobSchedulerInternal::TempJobAllocatorImpl;
        JobScheduler::Initialize();
        for (auto itr : JobSchedulerInternal::TempJobAllocatorImpl)
                int pause33333 = 3;
        auto i = JobSchedulerInternal::TempJobAllocatorImpl[0].Allocate();
        auto  j     = JobSchedulerInternal::StaticJobAllocatorImpl[0].Allocate();
        auto  addr = &i;
        auto  addr2 = &j;
        return 0;
}
