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
                GREEN_LIGHTS,
                BLUE_LIGHTS,
                WHITE_LIGHTS,
                COUNT
        };

        // BGRA colors
        static const DirectX::PackedVector::XMCOLOR ORB_COLORS[E_LIGHT_ORBS::COUNT];
        static const DirectX::PackedVector::XMCOLOR ORB_EDGE_COLORS[E_LIGHT_ORBS::COUNT];
        static const DirectX::PackedVector::XMCOLOR RING_COLORS[3];
        static const KeyCode                        ColorInputKeyCodes[E_LIGHT_ORBS::COUNT];
        static constexpr char                       Literal[E_LIGHT_ORBS::COUNT][6] = {"RED", "GREEN", "BLUE", "WHITE"};
};

// Red Color
extern DirectX::PackedVector::XMCOLOR C_RED;

// Red Ring Color
extern DirectX::PackedVector::XMCOLOR C_RED_RING;

// Green Color
extern DirectX::PackedVector::XMCOLOR C_GREEN;

// Green Ring Color
extern DirectX::PackedVector::XMCOLOR C_GREEN_RING;

// Blue Color
extern DirectX::PackedVector::XMCOLOR C_BLUE;

// Blue Ring Color
extern DirectX::PackedVector::XMCOLOR C_BLUE_RING;

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
