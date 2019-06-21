#include "ComputeShader.h"
#include<d3d11.h>
#include"../../Utility/Macros/DirectXMacros.h"
void ComputeShader::Release()
{
        SAFE_RELEASE(m_ComputerShader);
}
