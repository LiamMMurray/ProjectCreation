#pragma once
#include "../ECS/Component.h"
#include <DirectXMath.h>

class PhysicsComponent : public Component<PhysicsComponent>
{
    private:
        float				m_Mass		  = 0.0f;
        float				m_InverseMass = 0.0f;
        DirectX::XMVECTOR	m_Velocity    = DirectX::XMVectorSet(0, 0, 0, 0);
        DirectX::XMVECTOR   m_Force       = DirectX::XMVectorSet(0, 0, 0, 0);
        bool				m_UsesGravity = false;
		
    public:
        void				AddForce(DirectX::XMVECTOR force);
        void                ApplyForce(DirectX::XMVECTOR force);
        DirectX::XMVECTOR   GetForce();

        void				SetMass(float _Mass);
        float				GetMass();

        void				SetInverseMass(float _Mass);
        float				GetInverseMass();

        void				SetVelocity(DirectX::XMVECTOR _Velocity);
        DirectX::XMVECTOR	GetVelocity();

        bool				UsesGravity();
};