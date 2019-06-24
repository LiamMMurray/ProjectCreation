#pragma once

#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

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
};
