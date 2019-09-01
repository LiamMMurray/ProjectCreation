#pragma once

#include <D3DNativeTypes.h>


class PostProcessEffectBase
{
    protected:

        ID3D11Device1* m_Device;
        ID3D11DeviceContext1* m_Context;
    public:
        virtual void Initialize(ID3D11Device1* device, ID3D11DeviceContext1* context, D3D11_TEXTURE2D_DESC* desc) = 0;

        virtual void Render(ID3D11ShaderResourceView** inSRV, ID3D11RenderTargetView** outRTV) = 0;

        virtual void Shutdown() = 0;
};