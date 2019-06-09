#include "ParticleBufferSetup.h"
#include<vector>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#define NUM_ELEMENT 500
using namespace std;
void BufferSetup::ShaderSetup()
{

        int                tempSize = 0;
        CD3D11_BUFFER_DESC sbDesc;
        sbDesc.BindFlags           = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        sbDesc.CPUAccessFlags      = 0;
        sbDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        sbDesc.StructureByteStride = sizeof(tempSize);
        sbDesc.ByteWidth           = sizeof(tempSize) * NUM_ELEMENT * 1;
        sbDesc.Usage               = D3D11_USAGE_DEFAULT;

        D3D11_UNORDERED_ACCESS_VIEW_DESC sbUAVDesc;
        sbUAVDesc.Buffer.FirstElement = 0;
        sbUAVDesc.Buffer.Flags        = 0;
        sbUAVDesc.Buffer.NumElements  = NUM_ELEMENT * 1;
        sbUAVDesc.Format              = DXGI_FORMAT_UNKNOWN;
        sbUAVDesc.ViewDimension       = D3D11_UAV_DIMENSION_BUFFER;
        //HREFTYPE hr = m_device->CreateUnorderedAccessView(m_StructuredBuffer.Get(), &sbUAVDesc, m_UAV.GetAddressOf());

        D3D11_SHADER_RESOURCE_VIEW_DESC sbSRVDesc;
        sbSRVDesc.Buffer.ElementOffset = 0;
        sbSRVDesc.Buffer.ElementWidth  = sizeof(tempSize);
        sbSRVDesc.Buffer.FirstElement  = 0;
        sbSRVDesc.Buffer.NumElements   = NUM_ELEMENT * 1;
        sbSRVDesc.Format               = DXGI_FORMAT_UNKNOWN;
        sbSRVDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;
       // hr = m_Device->CreateShaderResourceView(m_StructuredBuffer.Get(), &sbSRVDesc, &m_StructuredView);

		
        vector<byte> bytes;
		string filePath;
       // if (ShaderUtilities::LoadShaderFromFile(fullPath, bytes))
        {
                //hr = m_Device->CreateComputeShader(bytes.data(), bytes.size(), nullptr, &m_ComputeShader);
        }
}
