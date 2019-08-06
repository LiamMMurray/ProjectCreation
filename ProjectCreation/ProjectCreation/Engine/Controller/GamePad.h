#pragma once
#include <Windows.h>
#include <Xinput.h>

#define MAX_RUMBLE 65535;

class GamePad
{
        XINPUT_STATE      m_InputState;
        XINPUT_VIBRATION* m_GamePadRumble;

    public:
        GamePad();

        void TestGamePadRumble();
};