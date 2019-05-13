#pragma once

#include "Resource.h"

#include "../Utility/ForwardDeclarations/D3DNativeTypes.h"



class Material : public Resource<Material>
{
        ID3D11VertexShader* m_VertexShader;
        ID3D11PixelShader*  m_PixelShader;
};