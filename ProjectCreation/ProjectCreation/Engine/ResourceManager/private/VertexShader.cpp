#include "VertexShader.h"

#include <d3d11_1.h>
#include <DirectXMacros.h>

void VertexShader::Release()
{
        SAFE_RELEASE(m_VertexShader);
}
