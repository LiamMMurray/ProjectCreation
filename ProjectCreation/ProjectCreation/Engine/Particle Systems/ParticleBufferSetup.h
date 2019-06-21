#pragma once
#include "../../Rendering/Vertex.h"
#include "../../Utility/ForwardDeclarations/D3DNativeTypes.h"
#include "ParticleData.h"
class BufferSetup
{
        friend class ParticleManager;
        static ID3D11Buffer*              m_StructuredBuffer;
        static ID3D11UnorderedAccessView* m_UAV;
        static ID3D11ShaderResourceView*  m_StructuredView;

    public:
        static BufferSetup* GetBufferData();
        void                InitBuffer();
        static void ComputeShaderSetup(ID3D11Device1* device1, ParticleData::FParticleGPU* particleData, int numParticles);
        void                ShutdownBuffer();
};