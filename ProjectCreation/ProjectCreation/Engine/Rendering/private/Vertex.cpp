#include "Vertex.h"

FVertex::FVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 color)
{
        FVertex::position = position;
        FVertex::color = color;
}

FSkinnedVertex::FSkinnedVertex(DirectX::XMFLOAT3 _p, DirectX::XMFLOAT4 _c)
{
        FSkinnedVertex::position = position;
        FSkinnedVertex::color    = color;
}
