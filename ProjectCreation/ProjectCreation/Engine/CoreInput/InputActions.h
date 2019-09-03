#pragma once
#include <JGamePad.h>
#include "CoreInput.h"

struct E_ACTION_STATE
{
        enum
        {
                Unpressed,
                DownFirst,
                Release,
                Down,
        };
};

struct E_ACTIONS
{
        enum
        {
                Red,
                Green,
                Blue
        };
};

struct E_AXIS
{
        enum
        {
                Move
        };
};

class InputActions
{
        InputActions();

    public:
        static constexpr uint16_t MKBInputs[] = {(uint16_t)KeyCode::A, (uint16_t)KeyCode::S, (uint16_t)KeyCode::D};

        static constexpr uint16_t GamepadInputs[] = {(uint16_t)ControllerCode::BUTTON_B,
                                                     (uint16_t)ControllerCode::BUTTON_A,
                                                     (uint16_t)ControllerCode::BUTTON_X};

        // static constexpr uint16_t GamepadInputs[] = {(uint16_t)ControllerCode::DPAD_LEFT,
        //                                             (uint16_t)ControllerCode::DPAD_DOWN,
        //                                             (uint16_t)ControllerCode::DPAD_RIGHT};


        static uint16_t CheckAction(uint16_t action);
        static uint16_t PauseAction();
        static uint16_t MouseClickAction();
        static uint16_t MoveAction();
        static uint16_t EnterAction();
        static float    CheckAxis(uint16_t axis);
};