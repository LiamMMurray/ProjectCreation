#pragma once

#include "Resource.h"

#include "../../Utility/ForwardDeclarations/D3DNativeTypes.h"

struct Texture2D : public Resource<Texture2D>
{
        ID3D11ShaderResourceView* m_SRV = nullptr;

        virtual void Release() override;
};