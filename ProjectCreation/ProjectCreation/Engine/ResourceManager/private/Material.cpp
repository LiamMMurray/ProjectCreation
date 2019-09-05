#include <Material.h>
#include <algorithm>

#include <d3d11.h>

#include <DirectXMacros.h>

#include <GEngine.h>

#include <PixelShader.h>
#include <VertexShader.h>


void Material::Copy(IResource* _other)
{
        Material* other      = (Material*)_other;
        m_VertexShaderHandle = other->m_VertexShaderHandle;
        m_PixelShaderHandle  = other->m_PixelShaderHandle;
        m_MaterialType       = other->m_MaterialType;
        m_SurfaceProperties  = other->m_SurfaceProperties;

        std::copy(std::begin(other->m_TextureHandles), std::end(other->m_TextureHandles), std::begin(m_TextureHandles));
}

void Material::Release()
{
        auto rm = GEngine::Get()->GetResourceManager();

        // rm->ReleaseResource<VertexShader>(m_VertexShaderHandle);
        // rm->ReleaseResource<PixelShader>(m_PixelShaderHandle);
}
