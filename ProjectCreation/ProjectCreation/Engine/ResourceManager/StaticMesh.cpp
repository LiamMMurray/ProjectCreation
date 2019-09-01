#include "StaticMesh.h"

#include <d3d11.h>

#include <DirectXMacros.h>

void StaticMesh::Release()
{
        SAFE_RELEASE(m_VertexBuffer);
        SAFE_RELEASE(m_IndexBuffer);
}
