#include <GeometryShader.h>
#include <d3d11.h>
#include <DirectXMacros.h>

void GeometryShader::Release()
{
        SAFE_RELEASE(m_GeometryShader);
}
