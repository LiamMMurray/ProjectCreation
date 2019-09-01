#pragma once
#include <Component.h>
#include "Quaternion.h"
struct FTransform
{
        DirectX::XMVECTOR translation;
        DirectX::XMVECTOR scale;
        FQuaternion       rotation;

        FTransform();
        FTransform(DirectX::XMVECTOR translation, DirectX::XMVECTOR scale, FQuaternion quaternion);
        DirectX::XMMATRIX CreateMatrix() const;
        void              SetMatrix(const DirectX::XMMATRIX& matrix);
        DirectX::XMVECTOR GetForward();
        DirectX::XMVECTOR GetRight();
        DirectX::XMVECTOR GetUp();
        void              RotateAxisAngle(DirectX::XMVECTOR& axis, float angle);
        static FTransform Lerp(const FTransform& lhs, const FTransform& rhs, float ratio);

		float GetRadius();

        inline void SetScale(const DirectX::XMVECTOR& _scale)
        {
                scale = _scale;
        }

        inline void SetScale(float _scale)
        {
                scale = DirectX::XMVectorSet(_scale, _scale, _scale, 1.0f);
        }
};