#pragma once
#include <Windows.h>
#include <unordered_map>
#include "Xinput.h"

class GamePad
{
        XINPUT_STATE     m_InputState;
        XINPUT_STATE     m_PrevInputState;
        XINPUT_GAMEPAD   m_InputGamePad;
        XINPUT_VIBRATION m_GamePadRumble;
        float              MAX_RUMBLE;


    public:
        GamePad();
        static GamePad* instance;

        void           IsVibrating(float strength);
        const uint16_t IsPressed(const WORD button) const;
        bool           CheckConnection();
        bool           Refresh();

        float normLX;
        float normLY;
        float deadzoneX;
        float deadzoneY;
        float leftStickX;
        float leftStickY;
        float rightStickX;
        float rightStickY;
        float leftTrigger;
        float rightTrigger;
        int   cId;

        static void Init();

        static void Shutdown();

        static GamePad* Get();

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
                return m_GamePadRumble;
        }

        inline void SetUpInput(XINPUT_STATE& inputState, XINPUT_GAMEPAD& gamePad, XINPUT_VIBRATION& vibration)
        {
                inputState = GetInputState();
                gamePad    = GetGamePad();
                vibration  = GetVibration();
        }

        std::unordered_map<uint16_t, uint16_t> xInputToKeyState;
};
