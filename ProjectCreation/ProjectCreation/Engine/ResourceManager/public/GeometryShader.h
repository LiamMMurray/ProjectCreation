#pragma once
#include <D3DNativeTypes.h>
#include "Resource.h"
struct GeometryShader : public Resource<GeometryShader>
{
        ID3D11GeometryShader* m_GeometryShader = nullptr;
        virtual void          Release() override;
};