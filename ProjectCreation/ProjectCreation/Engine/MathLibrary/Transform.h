#pragma once
#include "Quaternion.h"
#include "../../ECS/Component.h"
struct FTransform
{
        DirectX::XMVECTOR translation;
        DirectX::XMVECTOR scale;
        FQuaternion       rotation;

        FTransform();
        DirectX::XMMATRIX CreateMatrix() const;
        void              SetMatrix(const DirectX::XMMATRIX& matrix);
        DirectX::XMVECTOR GetForward();
        DirectX::XMVECTOR GetRight();
        DirectX::XMVECTOR GetUp();
        void       RotateAxisAngle(DirectX::XMVECTOR& axis, float angle);
        static FTransform Lerp(const FTransform& lhs, const FTransform& rhs, float ratio);
};