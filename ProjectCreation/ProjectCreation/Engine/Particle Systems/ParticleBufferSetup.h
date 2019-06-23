#pragma once
#include "../../Rendering/Vertex.h"
#include "../../Utility/ForwardDeclarations/D3DNativeTypes.h"
#include "ParticleData.h"

struct ParticleBuffer
{
        ID3D11Buffer*              m_StructuredBuffer;
        ID3D11UnorderedAccessView* m_UAV;
        ID3D11ShaderResourceView*  m_StructuredView;
};
struct Emitterbuffer
{
        ID3D11Buffer* m_StructuredBuffer;
        ID3D11ShaderResourceView* m_StructuredView;
};