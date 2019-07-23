#pragma once

#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "..//CoreInput/CoreInput.h"

struct E_LIGHT_ORBS
{
        enum
        {
                RED_LIGHTS = 0,
                BLUE_LIGHTS,
                GREEN_LIGHTS,
                WHITE_LIGHTS,
                COUNT
        };

        // BGRA colors
        static const DirectX::PackedVector::XMCOLOR ORB_COLORS[E_LIGHT_ORBS::COUNT];
        static const KeyCode ColorInputKeyCodes[E_LIGHT_ORBS::COUNT];
};

// Red Color
extern DirectX::PackedVector::XMCOLOR C_RED;

// Green Color
extern DirectX::PackedVector::XMCOLOR C_GREEN;

// Blue Color
extern DirectX::PackedVector::XMCOLOR C_BLUE;

// Yellow Color
extern DirectX::PackedVector::XMCOLOR C_YELLOW;

// Cyan Color
extern DirectX::PackedVector::XMCOLOR C_CYAN;

// Magenta Color
extern DirectX::PackedVector::XMCOLOR C_MAGENTA;

// Orange Color
extern DirectX::PackedVector::XMCOLOR C_ORANGE;

// White Color
extern DirectX::PackedVector::XMCOLOR C_WHITE;

// Black Color
extern DirectX::PackedVector::XMCOLOR C_BLACK;
