#pragma once

#include "Resource.h"

#include "../Utility/ForwardDeclarations/D3DNativeTypes.h"

struct PixelShader : public Resource<PixelShader>
{
        ID3D11PixelShader* m_PixelShader = nullptr;

        virtual void Release() override;
};