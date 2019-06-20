#include "AISystem.h"
#include "AIComponent.h"

XMVECTOR AISystem::CalculateAlignment(AIComponent* boid)
{
        XMVECTOR temp = m_AverageForward / boid->m_MaxVelocity;
        if (*XMVector3Length(temp).m128_f32 > 1) // Magnitude
        {
                XMVector3Normalize(temp);
        }
        return temp * m_AlignmentStrength;
}

XMVECTOR AISystem::CalculateCohesion(AIComponent* boid)
{
        XMVECTOR temp     = m_AveragePosition - boid->m_Position;
        float    distance = *XMVector3Length(temp).m128_f32; // Magnitude
        XMVector3Normalize(temp);

        // Set the speed based on distance from the flock.
        if (distance < m_FlockRadius)
        {
                temp *= distance / m_FlockRadius;
        }
        return temp * m_CohesionStrength;
}

XMVECTOR AISystem::CalculateSeperation(AIComponent* boid)
{
        XMVECTOR sum = {0, 0, 0};

        for (int i = 0; i < m_Boids.size(); i++)
        {
                XMVECTOR temp         = m_Boids[i]->m_Position - m_Boids[i]->m_Position;
                float    distance     = *XMVector3Length(temp).m128_f32; // Magnitude
                float    safeDistance = m_Boids[i]->m_SafeRadius + m_Boids[i]->m_SafeRadius;

                // If a collision is likely...
                if (distance < safeDistance)
                {
                        // Scale according to distance between boids.
                        XMVector3Normalize(temp);
                        temp *= (safeDistance - distance) / safeDistance;
                        sum += temp;
                }
        }

        if (*XMVector3Length(sum).m128_f32 > 1.0f) // Length
        {
                XMVector3Normalize(sum);
        }

        return sum * m_SeperationStrength;
}

void AISystem::CalculateAverage()
{
        m_AveragePosition = XMVectorZero();
        m_AverageForward  = XMVectorZero();

        for (int i = 0; i < m_Boids.size(); i++)
        {
                m_AverageForward += m_Boids[i]->m_Velocity;
        }
        m_AverageForward /= m_Boids.size();

        for (int i = 0; i < m_Boids.size(); i++)
        {
                m_AveragePosition += m_Boids[i]->m_Position;
        }
        m_AveragePosition /= m_Boids.size();
}

void AISystem::AddBoid(float Position, float )
{
       //AIComponent* BoidAdd = new AIComponent();
       //BoidAdd->m_Position  = Position;
	   //
       //m_Boids.push_back();
};


void AISystem::OnInitialize()
{
        m_AlignmentStrength  = 0.0f;
        m_CohesionStrength   = 0.0f;
        m_SeperationStrength = 0.0f;

        m_AveragePosition = XMVectorZero();
        m_AverageForward  = XMVectorZero();

        m_FlockRadius = 0.0f;
}

void AISystem::OnPreUpdate(float deltaTime)
{}

void AISystem::OnUpdate(float deltaTime)
{
        CalculateAverage();

        for (int i = 0; i < m_Boids.size(); i++)
        {
                XMVECTOR accel = CalculateAlignment(m_Boids[i]);
                accel += CalculateCohesion(m_Boids[i]);
                accel += CalculateSeperation(m_Boids[i]);

                float accelMultiplier = *m_Boids[i]->m_MaxVelocity.m128_f32;
                accel *= accelMultiplier * deltaTime;

                m_Boids[i]->m_Velocity += accel;

                if (m_Boids[i]->m_Velocity.m128_f32 > m_Boids[i]->m_MaxVelocity.m128_f32) // Magnitude
                {
                        XMVector3Normalize(m_Boids[i]->m_Velocity);
                        m_Boids[i]->m_Velocity *= m_Boids[i]->m_MaxVelocity;
                }
                m_Boids[i]->Update(deltaTime);
        }
}

void AISystem::OnPostUpdate(float deltaTime)
{}

void AISystem::OnShutdown()
{}

void AISystem::OnResume()
{}

void AISystem::OnSuspend()
{}
