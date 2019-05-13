#include "../Components/TestComponents.h"
#include "../Engine/GEngine.h"
#include "../Entities/TestEntities.h"
#include "ECS.h"
//	comment out to disable tests
// ____________________________________________________________________________

//#define ENABLE_ECS_TESTS

//	commentcomment out to disable individual tests
// ____________________________________________________________________________

#define TYPEID_TESTS
#define INSTANCING_TESTS
#define ENTITY_MANAGER_TESTS

#ifdef ENABLE_ECS_TESTS

#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#include <Windows.h>
#include <windowsx.h>
using namespace ECSTests;
// Windows version of main. WINAPI reverses order of parameters
int WinMain(HINSTANCE hInstance,     // ptr to current instance of app
            HINSTANCE hPrevInstance, // prev instance of app. Is always null nowadays.
            LPSTR     lpCmdLine,     // command line of app excluding program name
            int       nCmdShow       // how the windows is shown. Legacy. Can ignore
)
{

#ifdef TYPEID_TESTS
        // it is a compile time-error to perform operations between typeids belonging to different primary types
        //				ComponentTypeId compile_time_error = Entity<float>::GetTypeId();

        auto test_b1 = Key();
        auto test_b2 = Target();

        // Note: these two classes are equivalent
        /*1*/ auto test_b3   = ECSTests::Wisp();
        /*2*/ auto test_b4   = Entity<ECSTests::Wisp>();
        /* */ auto test_b3id = test_b3.GetTypeId();
        /* */ auto test_b4id = test_b4.GetTypeId();

        auto test_b5 = Skeleton();
        auto test_b6 = Key();
        auto test_b7 = Key();

        // Note: the TypeId incrementation follows the order of which class T in Entity<T> was declared first, not the
        // order GetTypeId() was called
        //
        // (Component classes and Entity classes must derive from Component<T> and Entity<T> respectively see TestComponents.h
        //  or TestEntities.h for examples)
        //
        // The following is a build time error because char and bool do not inherit from Component
        //		ComponentTypeId test_19 = Component<char>::GetTypeId();
        //		auto            test_20 = Component<bool>::GetTypeId();


        auto test_25 = Key::GetTypeId();
        auto test_28 = Target::GetTypeId();
        auto test_29 = test_b1.GetTypeId();
        auto test_30 = test_b2.GetTypeId();
        auto test_31 = test_b3.GetTypeId();

#endif
#ifdef ENTITY_MANAGER_TESTS
        GEngine::Initialize();
        EntityManager*    em = GEngine::Get()->GetEntityManager();
        ComponentManager* cm = GEngine::Get()->GetComponentManager();
        EntityHandle      a  = em->CreateEntity<Wisp>();
        EntityHandle      b  = em->CreateEntity<Wisp>();
        EntityHandle      c  = em->CreateEntity<Wisp>();
        EntityHandle      d  = em->CreateEntity<Skeleton>();
        EntityHandle      e  = em->CreateEntity<Skeleton>();
        EntityHandle      f  = em->CreateEntity<Skeleton>();
        EntityHandle      g  = em->CreateEntity<Wisp>();
        IEntity*          h  = em->GetEntity(a);
        IEntity*          i  = em->GetEntity(b);
        IEntity*          j  = em->GetEntity(c);
        IEntity*          k  = em->GetEntity(d);

        cm->AddComponent<Transform>(a);
        Transform* ta = cm->GetComponent<Transform>(a);
        Mesh*      ma = cm->GetComponent<Mesh>(a);
        ta->Bar();
        ta->Foo();
        // auto            test_a = em->GetEntity<Entity<Wisp>>(a);
        // throws build time error because float does not derive from CRTP Entity class
        // auto b  = em->CreateEntity<float>();
        // em->GetEntity()
#endif
        return 0;
}
#endif