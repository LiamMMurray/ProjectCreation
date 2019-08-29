#include "GamePad.h"
//#include <winerror.h>
//#include <d3d11.h>
#include "../ConsoleWindow/ConsoleWindow.h"
#include "../GEngine.h"

GamePad* GamePad::instance = nullptr;

GamePad::GamePad()
{
        DWORD dwResult;
        for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
        {
                ZeroMemory(&m_InputState, sizeof(XINPUT_STATE));

                // Simply get the state of the controller from XInput.
                dwResult = XInputGetState(i, &m_InputState);
                if (dwResult == ERROR_SUCCESS)
                {
                        // Controller is connected
                        ConsoleWindow::PrintMessage("Controller Is Connected", "GamePad");
                }
                else
                {
                        // Controller is not connected
                        ConsoleWindow::PrintMessage("Controller Is Not Connected", "GamePad");
                }
        }

        m_InputGamePad = m_InputState.Gamepad;

        ZeroMemory(&m_PrevInputState, sizeof(XINPUT_STATE));

        xInputToKeyState.insert(std::make_pair(0, (uint16_t)KeyState::Unpressed));
        xInputToKeyState.insert(std::make_pair(XINPUT_KEYSTROKE_KEYDOWN, (uint16_t)KeyState::DownFirst));
        xInputToKeyState.insert(std::make_pair(XINPUT_KEYSTROKE_KEYUP, (uint16_t)KeyState::Release));
        xInputToKeyState.insert(std::make_pair(XINPUT_KEYSTROKE_REPEAT, (uint16_t)KeyState::Down));
}

void GamePad::IsVibrating(float strengthL, float strengthR)
{
        instance->m_GamePadRumble.wLeftMotorSpeed  = WORD(MAX_RUMBLE * strengthL);
        instance->m_GamePadRumble.wRightMotorSpeed = WORD(MAX_RUMBLE * strengthR);
        XInputSetState(instance->cId, &instance->m_GamePadRumble);
}

const uint16_t GamePad::IsPressed(const WORD button) const
{
        uint16_t output     = (m_InputState.Gamepad.wButtons & button) != 0;
        uint16_t prevOutput = (m_PrevInputState.Gamepad.wButtons & button) != 0;

        // 0 = Unpressed
        // 1 = Down
        // 2 = Release
        // 3 = DownFirst

        // Button Held Down
        if (output == 1 && prevOutput == 1)
        {
                return (uint16_t)KeyState::Down;
        }

        // Button Was Just Pressed
        if ((output == 1 && prevOutput == 0))
        {
                return (uint16_t)KeyState::DownFirst;
        }

        // Button Was Just Released
        if ((output == 0 && prevOutput == 1))
        {
                return (uint16_t)KeyState::Release;
        }

        return (uint16_t)KeyState::Unpressed;
}

bool GamePad::CheckConnection()
{
        int controllerId = -1;

        for (DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++)
        {
                XINPUT_STATE state;
                ZeroMemory(&state, sizeof(XINPUT_STATE));

                if (XInputGetState(i, &state) == ERROR_SUCCESS)
                        controllerId = i;
        }

        cId = controllerId;

        return controllerId != -1;
}

// Returns false if the controller has been disconnected
bool GamePad::Refresh()
{
        if (instance->cId == -1)
                CheckConnection();

        if (instance->cId != -1)
        {
                m_PrevInputState = m_InputState;
                ZeroMemory(&instance->m_InputState, sizeof(XINPUT_STATE));
                if (XInputGetState(instance->cId, &instance->m_InputState) != ERROR_SUCCESS)
                {
                        instance->cId = -1;
                        return false;
                }

                float normLX = fmaxf(-1, (float)instance->m_InputState.Gamepad.sThumbLX / 32767);
                float normLY = fmaxf(-1, (float)instance->m_InputState.Gamepad.sThumbLY / 32767);

                instance->leftStickX =
                    (abs(normLX) < instance->deadzoneX ? 0 : (abs(normLX) - instance->deadzoneX) * (normLX / abs(normLX)));
                instance->leftStickY =
                    (abs(normLY) < instance->deadzoneY ? 0 : (abs(normLY) - instance->deadzoneY) * (normLY / abs(normLY)));

                if (instance->deadzoneX > 0)
                        instance->leftStickX *= 1 / (1 - instance->deadzoneX);
                if (instance->deadzoneY > 0)
                        instance->leftStickY *= 1 / (1 - instance->deadzoneY);

                float normRX = fmaxf(-1, (float)instance->m_InputState.Gamepad.sThumbRX / 32767);
                float normRY = fmaxf(-1, (float)instance->m_InputState.Gamepad.sThumbRY / 32767);

                instance->rightStickX =
                    (abs(normRX) < instance->deadzoneX ? 0 : (abs(normRX) - instance->deadzoneX) * (normRX / abs(normRX)));
                instance->rightStickY =
                    (abs(normRY) < instance->deadzoneY ? 0 : (abs(normRY) - instance->deadzoneY) * (normRY / abs(normRY)));

                if (instance->deadzoneX > 0)
                        instance->rightStickX *= 1 / (1 - instance->deadzoneX);
                if (instance->deadzoneY > 0)
                        instance->rightStickY *= 1 / (1 - instance->deadzoneY);

                instance->leftTrigger  = (float)instance->m_InputState.Gamepad.bLeftTrigger / 255;
                instance->rightTrigger = (float)instance->m_InputState.Gamepad.bRightTrigger / 255;

                return true;
        }
        return false;
}

GamePad* GamePad::Get()
{
        return instance;
}

void GamePad::Init()
{
        instance             = new GamePad();
        instance->MAX_RUMBLE = 65535.0f;
        instance->normLX     = fmaxf(-1, (float)instance->m_InputState.Gamepad.sThumbLX / 32767);
        instance->normLY     = fmaxf(-1, (float)instance->m_InputState.Gamepad.sThumbLY / 32767);
        instance->deadzoneX  = 0.1f;
        instance->deadzoneY  = 0.05f;
        instance->leftStickX = (abs(instance->normLX) < instance->deadzoneX ? 0 : instance->normLX);
        instance->leftStickY = (abs(instance->normLY) < instance->deadzoneY ? 0 : instance->normLY);
        instance->cId        = -1;
}

void GamePad::Shutdown()
{
        delete instance;
}
