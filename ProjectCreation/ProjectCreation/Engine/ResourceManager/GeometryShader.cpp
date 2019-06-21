#include "GeometryShader.h"
#include<d3d11.h>
#include"../../Utility/Macros/DirectXMacros.h"
void GeometryShader::Release()
{
        SAFE_RELEASE(m_GeometryShader);
}
