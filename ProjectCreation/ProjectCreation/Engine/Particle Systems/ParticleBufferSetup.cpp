#include "ParticleBufferSetup.h"
#include <DirectXPackedVector.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <string>
#include <vector>
#include "../../Utility/Macros/DirectXMacros.h"
#include "ParticleData.h"

//#pragma comment(lib, "d3d11.lib")
#define NUM_ELEMENT 50
using namespace std;
using namespace ParticleData;
BufferSetup* BufferSetup::GetBufferData()
{
        BufferSetup data;
        return &data;
}
void BufferSetup::InitBuffer()
{
        m_StructuredBuffer = nullptr;

        m_UAV = nullptr;

        m_StructuredView = nullptr;
}
void BufferSetup::ComputeShaderSetup(ID3D11Device1* device1, FParticleGPU* particlData, int numParticles)
{

        CD3D11_BUFFER_DESC sbDesc;
        sbDesc.BindFlags           = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        sbDesc.CPUAccessFlags      = 0;
        sbDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        sbDesc.StructureByteStride = sizeof(FParticleGPU);
        sbDesc.ByteWidth           = sizeof(FParticleGPU) * numParticles * 1;
        sbDesc.Usage               = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA rwData;
        rwData.pSysMem          = particlData; // not sure should pass in what type of data
        rwData.SysMemPitch      = 0;
        rwData.SysMemSlicePitch = 0;
        device1->CreateBuffer(&sbDesc, &rwData, &m_StructuredBuffer);

        D3D11_UNORDERED_ACCESS_VIEW_DESC sbUAVDesc;
        sbUAVDesc.Buffer.FirstElement = 0;
        sbUAVDesc.Buffer.Flags        = 0;
        sbUAVDesc.Buffer.NumElements  = numParticles * 1;
        sbUAVDesc.Format              = DXGI_FORMAT_UNKNOWN;
        sbUAVDesc.ViewDimension       = D3D11_UAV_DIMENSION_BUFFER;
        // HREFTYPE hr = m_Device->CreateUnorderedAccessView(m_StructuredBuffer.Get(), &sbUAVDesc, m_UAV.GetAddressOf());
        HREFTYPE hr = device1->CreateUnorderedAccessView(
            m_StructuredBuffer, &sbUAVDesc, &m_UAV); // if passing the &sbUAVDesc as nullptr, get the infprmation from the
                                                     // buffer and creates the viewDesc from it
        
		ZeroMemory(&sbDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));


        D3D11_SHADER_RESOURCE_VIEW_DESC sbSRVDesc;
        sbSRVDesc.Buffer.ElementOffset = 0;
        sbSRVDesc.Buffer.ElementWidth  = sizeof(FParticleGPU);
        sbSRVDesc.Buffer.FirstElement  = 0;
        sbSRVDesc.Buffer.NumElements   = numParticles * 1;
        sbSRVDesc.Format               = DXGI_FORMAT_UNKNOWN;
        sbSRVDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;
        hr                             = device1->CreateShaderResourceView(m_StructuredBuffer, &sbSRVDesc, &m_StructuredView);
}

void BufferSetup::ShutdownBuffer()
{
        SAFE_RELEASE(m_StructuredBuffer);
        SAFE_RELEASE(m_UAV);
        SAFE_RELEASE(m_StructuredView);
}
