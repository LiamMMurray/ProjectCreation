#pragma once
#include "../Utility/ForwardDeclarations/D3DNativeTypes.h"
#include "ParticleInfo.h"
class BufferSetup
{
        ID3D11Device1*        m_Device  = nullptr;
        ID3D11DeviceContext1* m_Context = nullptr;

    public:
        BufferSetup();
        void InitBuffer();
        void UpdateBuffer();
        void ShaderSetup();
        void ShutdownBuffer();
        void EmmitParticle();
};