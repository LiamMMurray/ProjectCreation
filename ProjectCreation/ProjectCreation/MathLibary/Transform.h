#pragma once
#include "Quaternion.h"
struct FTransform
{
        DirectX::XMVECTOR translation;
        FQuaternion       rotation;
        DirectX::XMVECTOR scale;

        FTransform();
        DirectX::XMMATRIX CreateMatrix() const;
        void              SetMatrix(const DirectX::XMMATRIX& matrix);
        DirectX::XMVECTOR GetForward();
        DirectX::XMVECTOR GetRight();
        DirectX::XMVECTOR GetUp();
        static void       RotateAxisAngle(DirectX::XMVECTOR& axis, float angle);
};