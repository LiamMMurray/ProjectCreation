#pragma once
#include <Windows.h>
#include <Xinput.h>
#include "../CoreInput/CoreInput.h"
#include "../GEngine.h"


class GamePad
{
        XINPUT_STATE      m_InputState;
        XINPUT_GAMEPAD    m_InputGamePad;
        XINPUT_VIBRATION* m_GamePadRumble;
        int               MAX_RUMBLE = 65535;

    public:
        GamePad();

        void       TestGamePadRumble();
        const bool IsPressed(const WORD button) const;
        bool       CheckConnection();
        bool       Refresh();

        float normLX = fmaxf(-1, (float)m_InputState.Gamepad.sThumbLX / 32767);
        float normLY = fmaxf(-1, (float)m_InputState.Gamepad.sThumbLY / 32767);

        float deadzoneX = 0.05f;
        float deadzoneY = 0.02f;

        float leftStickX = (abs(normLX) < deadzoneX ? 0 : normLX);
        float leftStickY = (abs(normLY) < deadzoneY ? 0 : normLY);

        float rightStickX;
        float rightStickY;
        float leftTrigger;
        float rightTrigger;
        int   cId = -1;
		
        inline XINPUT_GAMEPAD GetGamePad()
        {
                return m_InputGamePad;
        }

        inline XINPUT_STATE GetInputState()
        {
                return m_InputState;
        }

        inline XINPUT_VIBRATION GetVibration()
        {
                return *m_GamePadRumble;
        }

        inline void SetUpInput(XINPUT_STATE& inputState, XINPUT_GAMEPAD& gamePad, XINPUT_VIBRATION& vibration)
        {
                inputState = GetInputState();
                gamePad    = GetGamePad();
                vibration  = GetVibration();
        }
};
