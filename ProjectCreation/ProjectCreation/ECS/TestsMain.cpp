#include "ECS.h"

//	comment out to disable tests
// ____________________________________________________________________________

//#define ECS_TESTS

//	commentcomment out to disable individual tests
// ____________________________________________________________________________

#define TYPEID_TESTS
#define INSTANCING_TESTS


#ifdef ECS_TESTS

#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#include <Windows.h>
#include <windowsx.h>

int WinMain(HINSTANCE hInstance,     // ptr to current instance of app
                   HINSTANCE hPrevInstance, // prev instance of app. Is always null nowadays.
                   LPSTR     lpCmdLine,     // command line of app excluding program name
                   int       nCmdShow       // how the windows is shown. Legacy. Can ignore
)
{
        Entity<float> test_0;
        Entity<float> test_1;
        Entity<float> test_2;
        Entity<float> test_3;
        Entity<float> test_4;
        Entity<float> test_5;
        Entity<char>  test_6;
        Entity<char>  test_7;
        Entity<char>  test_8;
        Entity<float> test_9;
        Entity<float> test_10;
        Entity<float> test_11;
        Entity<float> test_12;
        Entity<int>   test_13;
        Entity<int>   test_14;
        Entity<int>   test_15;
        Entity<float> test_16;
        Entity<float> test_17;

#ifdef TYPEID_TESTS
        // it is a compile time-error to perform operations between typeids belonging to different primary types
        // TypeId<IComponent> compile_time_error = Entity<float>::GetTypeId();
        TypeId<IEntity>    test_18 = Entity<char>::GetTypeId();
        TypeId<IComponent> test_19 = Component<char>::GetTypeId();
        auto               test_20 = Component<bool>::GetTypeId();
        auto               test_21 = Component<int>::GetTypeId();
        auto               test_22 = Component<float>::GetTypeId();
        auto               test_23 = Component<float>::GetTypeId();
        auto               test_24 = Component<int>::GetTypeId();
        auto               test_25 = Entity<char>::GetTypeId();
        auto               test_26 = Component<int>::GetTypeId();
        auto               test_27 = Component<int>::GetTypeId();
        auto               test_28 = test_0.GetTypeId();
        auto               test_29 = test_12.GetTypeId();
        auto               test_30 = test_6.GetTypeId();
        auto               test_31 = test_3.GetTypeId();
#endif
        return 0;
}
#endif