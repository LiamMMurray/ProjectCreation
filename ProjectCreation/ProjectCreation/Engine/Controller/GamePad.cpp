#include "GamePad.h"
//#include <winerror.h>
//#include <d3d11.h>
#include "../ConsoleWindow/ConsoleWindow.h"

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
		
        m_GamePadRumble = new XINPUT_VIBRATION();

}

void GamePad::TestGamePadRumble()
{
        if (m_GamePadRumble != NULL)
        {
                int currRumble = MAX_RUMBLE*0.75f;

                while (currRumble > 0)
                {
                        m_GamePadRumble->wLeftMotorSpeed  = currRumble;
                        m_GamePadRumble->wRightMotorSpeed = currRumble;
						
						currRumble = MathLibrary::MoveTowards(currRumble, 0, GEngine::Get()->GetDeltaTime()*0.5f);
                        XInputSetState(0, m_GamePadRumble);

                }
        }
}

const bool GamePad::IsPressed(const WORD button) const
{
        return (m_InputState.Gamepad.wButtons & button) != 0;
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
        if (cId == -1)
                CheckConnection();

        if (cId != -1)
        {
                ZeroMemory(&m_InputState, sizeof(XINPUT_STATE));
                if (XInputGetState(cId, &m_InputState) != ERROR_SUCCESS)
                {
                        cId = -1;
                        return false;
                }

                float normLX = fmaxf(-1, (float)m_InputState.Gamepad.sThumbLX / 32767);
                float normLY = fmaxf(-1, (float)m_InputState.Gamepad.sThumbLY / 32767);

                leftStickX = (abs(normLX) < deadzoneX ? 0 : (abs(normLX) - deadzoneX) * (normLX / abs(normLX)));
                leftStickY = (abs(normLY) < deadzoneY ? 0 : (abs(normLY) - deadzoneY) * (normLY / abs(normLY)));

                if (deadzoneX > 0)
                        leftStickX *= 1 / (1 - deadzoneX);
                if (deadzoneY > 0)
                        leftStickY *= 1 / (1 - deadzoneY);

                float normRX = fmaxf(-1, (float)m_InputState.Gamepad.sThumbRX / 32767);
                float normRY = fmaxf(-1, (float)m_InputState.Gamepad.sThumbRY / 32767);

                rightStickX = (abs(normRX) < deadzoneX ? 0 : (abs(normRX) - deadzoneX) * (normRX / abs(normRX)));
                rightStickY = (abs(normRY) < deadzoneY ? 0 : (abs(normRY) - deadzoneY) * (normRY / abs(normRY)));

                if (deadzoneX > 0)
                        rightStickX *= 1 / (1 - deadzoneX);
                if (deadzoneY > 0)
                        rightStickY *= 1 / (1 - deadzoneY);

                leftTrigger  = (float)m_InputState.Gamepad.bLeftTrigger / 255;
                rightTrigger = (float)m_InputState.Gamepad.bRightTrigger / 255;

                return true;
        }
        return false;
}

