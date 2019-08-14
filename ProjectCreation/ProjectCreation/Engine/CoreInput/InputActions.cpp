#include "InputActions.h"

uint16_t InputActions::CheckAction(uint16_t action)
{
        uint16_t returnType;
        if (GamePad::Get()->CheckConnection() == true) // has controller
        {
                auto temp = (KeyState)GCoreInput::GetControllerState((ControllerCode)GamepadInputs[action]);
                return (uint16_t)GCoreInput::GetControllerState((ControllerCode)GamepadInputs[action]);
        }

        if (GCoreInput::GetKeyState((KeyCode)action) == KeyState::Down)
        {
                int temp = 0;
        }
        returnType = (uint16_t)GCoreInput::GetKeyState((KeyCode)MKBInputs[action]);

        return returnType;
}

uint16_t InputActions::PauseAction()
{
        uint16_t returnType;

        if (GamePad::Get()->CheckConnection() == true) // has controller
        {
                returnType = (KeyState)GCoreInput::GetControllerState(ControllerCode::BUTTON_START);
                if (returnType == KeyState::DownFirst)
                {
                        int temp = 0;
                }
                return returnType;
        }

        returnType = (uint16_t)GCoreInput::GetKeyState(KeyCode::Esc);

        return returnType;
}

uint16_t InputActions::MouseClickAction()
{
        uint16_t returnType;

        if (GamePad::Get()->CheckConnection() == true) // has controller
        {
                returnType = (KeyState)GCoreInput::GetControllerState(ControllerCode::BUTTON_A);
                if (returnType == KeyState::Release)
                {
                        int temp = 0;
                }
                return returnType;
        }

        returnType = (uint16_t)GCoreInput::GetMouseState(MouseCode::LeftClick);

        return returnType;
}

uint16_t InputActions::MoveAction()
{
        uint16_t returnType;

        if (GamePad::Get()->CheckConnection() == true) // has controller
        {
                returnType = (KeyState)GCoreInput::GetControllerState(ControllerCode::BUTTON_START);
                if (returnType == KeyState::DownFirst)
                {
                        int temp = 0;
                }
                return returnType;
        }

        returnType = (uint16_t)GCoreInput::GetKeyState(KeyCode::Esc);

        return returnType;
}

uint16_t InputActions::EnterAction()
{
        uint16_t returnType;

        if (GamePad::Get()->CheckConnection() == true) // has controller
        {
                returnType = (KeyState)GCoreInput::GetControllerState(ControllerCode::BUTTON_BACK);
                if (returnType == KeyState::DownFirst)
                {
                        int temp = 0;
                }
                return returnType;
        }

        returnType = (uint16_t)GCoreInput::GetKeyState(KeyCode::Enter);

        return returnType;
}
