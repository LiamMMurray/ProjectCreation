#pragma once
namespace MathConstants
{
        constexpr float kEpsilon = 0.001f;
}

class VectorConstants
{
    public:
        static constexpr DirectX::XMVECTORF32 Right   = {1.0f, 0.0f, 0.0f, 0.0f};
        static constexpr DirectX::XMVECTORF32 Up      = {0.0f, 1.0f, 0.0f, 0.0f};
        static constexpr DirectX::XMVECTORF32 Forward = {0.0f, 0.0f, 1.0f, 0.0f};
        static constexpr DirectX::XMVECTORF32 Zero    = {0.0f, 0.0f, 0.0f, 0.0f};
};