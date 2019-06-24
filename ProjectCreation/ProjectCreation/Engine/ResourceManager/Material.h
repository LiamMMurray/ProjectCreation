#pragma once

#include "Resource.h"

#include "../../Utility/ForwardDeclarations/D3DNativeTypes.h"

#include "ResourceInfoTypes.h"

struct Material : public Resource<Material>
{
        ResourceHandle            m_VertexShaderHandle;
        ResourceHandle            m_PixelShaderHandle;
        EMaterialType             m_MaterialType;
        FSurfaceProperties        m_SurfaceProperties;
        ResourceHandle            m_TextureHandles[int(ETexture2DType::COUNT)];

		Material() = default;
        virtual void Copy(IResource* other) override;

        virtual void Release() override;
};