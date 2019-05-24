#include "RenderUtility.h"

#include <d3d11_1.h>

void RenderUtility::UpdateConstantBuffer(ID3D11DeviceContext1* context, ID3D11Buffer* gpuBuffer, void* cpuBuffer, size_t size)
{
        D3D11_MAPPED_SUBRESOURCE mappedResource{};
        context->Map(gpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy(mappedResource.pData, cpuBuffer, size);
        context->Unmap(gpuBuffer, 0);
}
