#pragma once

#include "Resource.h"

#include "../Utility/ForwardDeclarations/D3DNativeTypes.h"

struct VertexShader : public Resource<VertexShader>
{
        ID3D11VertexShader* m_VertexShader = nullptr;

		virtual void Release() override;
};