#pragma once
#include "../../Utility/ForwardDeclarations/D3DNativeTypes.h"
#include "Resource.h"
struct ComputeShader : public Resource<ComputeShader>
{
        ID3D11ComputeShader* m_ComputerShader = nullptr;
        virtual void         Release()override;
};