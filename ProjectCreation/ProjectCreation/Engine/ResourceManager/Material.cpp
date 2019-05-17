#include "Material.h"
#include <algorithm>

#include <d3d11.h>

#include "../../Utility/Macros/DirectXMacros.h"

#include "../GEngine.h"

#include "VertexShader.h"
#include "PixelShader.h"

void Material::Release()
{
        auto rm = GEngine::Get()->GetResourceManager();

       rm->ReleaseResource<VertexShader>(m_VertexShaderHandle);
       rm->ReleaseResource<PixelShader>(m_PixelShaderHandle);
}
