/***********************************************
 * Filename: CoreInput.h
 * Date: 05/09/2019
 * Mod. Date: 05/09/2019
 * Mod. Initials: JV
 * Author: Jose Villarroel
 * Purpose: This is the input singleton.
 ************************************************/

#pragma once

#include <stdint.h>

#ifndef _WINDEF_
struct HINSTANCE__; // Forward or never
typedef HINSTANCE__* HINSTANCE;

typedef unsigned int   UINT;
typedef unsigned long  ULONG;
typedef unsigned short USHORT;

#if defined(_WIN64)
typedef __int64          INT_PTR, *PINT_PTR;
typedef unsigned __int64 UINT_PTR, *PUINT_PTR;
typedef __int64          LONG_PTR, *PLONG_PTR;
typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;
#else
typedef __w64 int           INT_PTR, *PINT_PTR;
typedef __w64 unsigned int  UINT_PTR, *PUINT_PTR;
typedef __w64 long          LONG_PTR, *PLONG_PTR;
typedef __w64 unsigned long ULONG_PTR, *PULONG_PTR;
#endif

typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

struct HWND__;
typedef HWND__* HWND;
#endif

// More keys will be added later. The number is based on VK_KEY.
// If you urgently need a key that is not available here, you can add it from
// https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes
enum class KeyCode
{
        Enter   = 0x0D,
        Space   = 0x20,
        Control = 0x11,
        Tab     = 0x09,
        Esc     = 0x1B,
        Shift   = 0x10,
        A       = 0x41,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Zero = 0x30,
        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Num0 = 0x60,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Num9,
        Num10,
        Plus  = 0x6B,
        Minus = 0x6D,
        Left  = 0x25,
        Up,
        Right,
        Down,
        Any  = -2,
        None = -1
};

enum class MouseCode
{
        LeftClick = 0,
        RightClick
};

enum KeyState
{
        Unpressed = 0,
        DownFirst,
        Release,
        Down
};

class GCoreInput
{
    public:
        // Initializes the input singleton. Must be called before the main loop
        static void InitializeInput(HWND hwnd);

        // Called within the winProc function, on the WM_INPUT message. Don't call anywhere else
        static void GatherInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        // Must be called once per frame, within the main loop. Allows us to know which input was consumed in
        // a given frame, since GatherInput may be called more than once per frame.
        static void UpdateInput();

        // Returns how many pixels the mouse has moved horizontally(positive or negative) since last frame.
        // When we implement a sensitivity value, this will become a float.
        inline static int32_t GetMouseX()
        {
                return mouseX;
        };

        // Returns how many pixels the mouse has moved vertically(positive or negative) since last frame.
        // When we implement a sensitivity value, this will become a float.
        inline static int32_t GetMouseY()
        {
                return mouseY;
        };
        static int32_t  GetMouseWindowPosX();
        static int32_t  GetMouseWindowPosY();
        static KeyState GetKeyState(KeyCode target);
        static KeyState GetMouseState(MouseCode target);

    private:
        static void    ResetAxes();
        static int32_t mouseX;
        static int32_t mouseWindowPosX;
        static int32_t mouseY;
        static int32_t mouseWindowPosY;
        static uint8_t keyStates[256];
        static uint8_t MouseStates[2];
};