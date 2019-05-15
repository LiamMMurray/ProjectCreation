#include "Texture2D.h"

#include <d3d11_1.h>
#include "../Utility/Macros/DirectXMacros.h"

void Texture2D::Release()
{
        SAFE_RELEASE(m_SRV);
}
