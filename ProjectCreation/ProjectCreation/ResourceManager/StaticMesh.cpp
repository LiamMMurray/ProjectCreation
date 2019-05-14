#include "StaticMesh.h"

#include <d3d11.h>

#include "../Utility/Macros/DirectXMacros.h"

void StaticMesh::Release()
{
        SAFE_RELEASE(m_VertexBuffer);
        SAFE_RELEASE(m_IndexBuffer);
}
