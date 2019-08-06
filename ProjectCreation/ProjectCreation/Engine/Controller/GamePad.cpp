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
        m_GamePadRumble = new XINPUT_VIBRATION();
}

void GamePad::TestGamePadRumble()
{
        if (m_GamePadRumble != NULL)
        {
                m_GamePadRumble->wLeftMotorSpeed  = MAX_RUMBLE;
                m_GamePadRumble->wRightMotorSpeed = MAX_RUMBLE;

                XInputSetState(0, m_GamePadRumble);
        }
}
