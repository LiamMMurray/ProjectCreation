#pragma once

#include "Resource.h"

#include "../Utility/ForwardDeclarations/D3DNativeTypes.h"

struct StaticMesh : public Resource<StaticMesh>
{
        virtual void Release() override;

        ID3D11Buffer* m_VertexBuffer;
        ID3D11Buffer* m_IndexBuffer;
        uint32_t      m_VertexCount;
        uint32_t      m_IndexCount;
};