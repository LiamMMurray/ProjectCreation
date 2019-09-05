#pragma once

#include <D3DNativeTypes.h>

#include <Resource.h>
#include <AnimationContainers.h>

struct SkeletalMesh : public Resource<SkeletalMesh>
{
        virtual void Release() override;

        ID3D11Buffer* m_VertexBuffer;
        ID3D11Buffer* m_IndexBuffer;
        uint32_t      m_VertexCount;
        uint32_t      m_IndexCount;

		Animation::FSkeleton m_BindPoseSkeleton;
};