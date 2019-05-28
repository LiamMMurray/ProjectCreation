#include "CoreInput.h"

#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#define NOMINMAX
#include <Windows.h>
#include <windowsx.h>

int32_t GCoreInput::mouseX = 0;
int32_t GCoreInput::mouseY = 0;
int32_t GCoreInput::mouseScreenPosX = 0;
int32_t GCoreInput::mouseScreenPosY = 0;
uint8_t GCoreInput::keyStates[256]{};
uint8_t GCoreInput::MouseStates[2]{};


void GCoreInput::InitializeInput(HWND hwnd)
{
        RAWINPUTDEVICE Rid[2];

        Rid[0].usUsagePage = 0x01;
        Rid[0].usUsage     = 0x02;
        Rid[0].dwFlags     = 0; // adds HID mouse and also ignores legacy mouse messages
        Rid[0].hwndTarget  = 0;

        Rid[1].usUsagePage = 0x01;
        Rid[1].usUsage     = 0x06;
        Rid[1].dwFlags     = 0; // adds HID keyboard and also ignores legacy keyboard messages
        Rid[1].hwndTarget  = 0;


        if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
        {
                DWORD error = GetLastError();
                return;
        }

        ZeroMemory(keyStates, sizeof(uint8_t) * 256);
}

void GCoreInput::ResetAxes()
{
        mouseX = 0;
        mouseY = 0;
}

void GCoreInput::GatherInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
        UINT     dwSize = sizeof(RAWINPUT);
        RAWINPUT raw{};

        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER));


        if (raw.header.dwType == RIM_TYPEMOUSE)
        {
                GCoreInput::mouseX += raw.data.mouse.lLastX;
                GCoreInput::mouseY += raw.data.mouse.lLastY;

                if (raw.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
                {
                        MouseStates[(int)MouseCode::LeftClick] |= 1;
                }
                if (raw.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
                {
                        MouseStates[(int)MouseCode::LeftClick] &= ~1;
                }

                if (raw.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
                {
                        MouseStates[(int)MouseCode::RightClick] |= 1;
                }
                if (raw.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
                {
                        MouseStates[(int)MouseCode::RightClick] &= ~1;
                }
        }

        if (raw.header.dwType == RIM_TYPEKEYBOARD)
        {
                keyStates[raw.data.keyboard.VKey] &= 0xfe;
                keyStates[raw.data.keyboard.VKey] |= 1 - (RI_KEY_BREAK & raw.data.keyboard.Flags);
        }

		POINT cursorPos;
        GetCursorPos(&cursorPos);
        mouseScreenPosX = cursorPos.x;
        mouseScreenPosY = cursorPos.y;
}

void GCoreInput::UpdateInput()
{
        for (int i = 0; i < 256; i++)
        {
                const uint8_t last = 1 & keyStates[i];
                keyStates[i]       = (keyStates[i] << 1) | last;
        }

        for (int i = 0; i < 2; i++)
        {
                const uint8_t last = 1 & MouseStates[i];
                MouseStates[i]     = (MouseStates[i] << 1) | last;
        }


        ResetAxes();
}

int32_t GCoreInput::GetMouseScreenPosX()
{
        return mouseScreenPosX;
}

int32_t GCoreInput::GetMouseScreenPosY()
{
        mouseScreenPosY;
}

KeyState GCoreInput::GetKeyState(KeyCode target)
{
        KeyState output = static_cast<KeyState>(0x3 & keyStates[static_cast<int>(target)]);
        return output;
}

KeyState GCoreInput::GetMouseState(MouseCode target)
{
        KeyState output = static_cast<KeyState>(0x3 & MouseStates[static_cast<int>(target)]);
        return output;
}
