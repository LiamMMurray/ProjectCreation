#include "VertexShader.h"

#include <d3d11_1.h>
#include "../../Utility/Macros/DirectXMacros.h"

void VertexShader::Release()
{
        SAFE_RELEASE(m_VertexShader);
}
