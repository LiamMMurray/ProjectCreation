#include "../Engine/GEngine.h"
#include "../Entities/TestEntities.h"
#include "ECS.h"
//	comment out to disable tests
// ____________________________________________________________________________

#define ENABLE_ECS_TESTS

//	commentcomment out to disable individual tests
// ____________________________________________________________________________

#define TYPEID_TESTS
#define INSTANCING_TESTS
#define ENTITY_MANAGER_TESTS

#ifdef ENABLE_ECS_TESTS

#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#include <Windows.h>
#include <windowsx.h>

// Windows version of main. WINAPI reverses order of parameters
int WinMain(HINSTANCE hInstance,     // ptr to current instance of app
            HINSTANCE hPrevInstance, // prev instance of app. Is always null nowadays.
            LPSTR     lpCmdLine,     // command line of app excluding program name
            int       nCmdShow       // how the windows is shown. Legacy. Can ignore
)
{

#ifdef TYPEID_TESTS
        // it is a compile time-error to perform operations between typeids belonging to different primary types
        // TypeId<IComponent> compile_time_error = Entity<float>::GetTypeId();
        auto test_b1 = Entity<Key>();
        auto test_b2 = Entity<Target>();
        auto test_b3 = Entity<Wisp>();
        auto test_b4 = Entity<Creature>();
        auto test_b5 = Entity<Skeleton>();
        auto test_b6 = Entity<Key>();
        auto test_b7 = Entity<Key>();

        TypeId<IEntity>    test_18 = Entity<Key>::GetTypeId();
        TypeId<IComponent> test_19 = Component<char>::GetTypeId();
        auto               test_20 = Component<bool>::GetTypeId();
        auto               test_21 = Component<int>::GetTypeId();
        auto               test_22 = Component<float>::GetTypeId();
        auto               test_23 = Component<float>::GetTypeId();
        auto               test_24 = Component<int>::GetTypeId();
        auto               test_25 = Entity<Key>::GetTypeId();
        auto               test_26 = Component<int>::GetTypeId();
        auto               test_27 = Component<int>::GetTypeId();
        auto               test_28 = Entity<Target>::GetTypeId();
        // Note: the TypeId incrementation follows the order of which class T in Entity<T> was declared first, not the
        // order GetTypeId() was called
        auto test_29 = test_b1.GetTypeId();
        auto test_30 = test_b2.GetTypeId();
        auto test_31 = test_b3.GetTypeId();

#endif
#ifdef ENTITY_MANAGER_TESTS
        GEngine::Initialize();
        auto            em     = GEngine::Get()->GetEntityManager();
        Handle<IEntity> a      = em->CreateEntity<Wisp>();
        Handle<IEntity> b      = em->CreateEntity<Wisp>();
        Handle<IEntity> c      = em->CreateEntity<Wisp>();
        Handle<IEntity> d      = em->CreateEntity<Skeleton>();
        Handle<IEntity> e      = em->CreateEntity<Skeleton>();
        Handle<IEntity> f      = em->CreateEntity<Skeleton>();
        Handle<IEntity> g      = em->CreateEntity<Wisp>();
        auto            test_a = em->GetEntity<Entity<Wisp>>(a);
        // throws build time error because float does not derive from CRTP Entity class
        // auto b  = em->CreateEntity<float>();
        // em->GetEntity()
#endif
        return 0;
}
#endif