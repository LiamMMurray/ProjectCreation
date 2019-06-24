#include "AISystem.h"
#include "../GenericComponents/TransformComponent.h"
#include "../MathLibrary/MathLibrary.h"
#include "AIComponent.h"

XMVECTOR AISystem::CalculateAlignment(AIComponent* boid)
{
        XMVECTOR temp = m_AverageForward / boid->m_MaxVelocity;
        if (MathLibrary::CalulateVectorLength(temp) > 1.0f) // Magnitude
        {
                XMVector3Normalize(temp);
        }
        return temp * m_AlignmentStrength;
}

XMVECTOR AISystem::CalculateCohesion(AIComponent* boid)
{
        XMVECTOR temp     = m_AveragePosition - boid->GetParent().GetComponent<TransformComponent>()->transform.translation;
        float    distance = MathLibrary::CalulateVectorLength(temp); // Magnitude
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
        XMVECTOR sum = XMVectorZero();

        for (auto& aiComp : m_HandleManager->GetActiveComponents<AIComponent>())
        {
                XMVECTOR myPos    = boid->GetParent().GetComponent<TransformComponent>()->transform.translation;
                XMVECTOR otherPos = aiComp.GetParent().GetComponent<TransformComponent>()->transform.translation;

                XMVECTOR temp         = myPos - otherPos;
                float    distance     = MathLibrary::CalulateVectorLength(temp); // Magnitude
                float    safeDistance = boid->m_SafeRadius + aiComp.m_SafeRadius;

                // If a collision is likely...
                if (distance < safeDistance)
                {
                        // Scale according to distance between boids.
                        XMVector3Normalize(temp);
                        temp *= (safeDistance - distance) / safeDistance;
                        sum += temp;
                }
        }

        if (MathLibrary::CalulateVectorLength(sum) > 1.0f) // Length
        {
                XMVector3Normalize(sum);
        }

        return sum * m_SeperationStrength;
}

void AISystem::CalculateAverage()
{
        m_AveragePosition = XMVectorZero();
        m_AverageForward  = XMVectorZero();

        int boidCount = 0;
        for (auto& aiComp : m_HandleManager->GetActiveComponents<AIComponent>())
        {
                boidCount++;
                m_AverageForward += aiComp.m_Velocity;
        }
        m_AverageForward /= boidCount;

        for (auto& aiComp : m_HandleManager->GetActiveComponents<AIComponent>())
        {
                m_AveragePosition += aiComp.GetParent().GetComponent<TransformComponent>()->transform.translation;
        }
        m_AveragePosition /= boidCount;
}


void AISystem::OnInitialize()
{
        m_AveragePosition = XMVectorZero();
        m_AverageForward  = XMVectorZero();

        m_AlignmentStrength  = 0.0f;
        m_CohesionStrength   = 0.0f;
        m_SeperationStrength = 1.5f;

        m_FlockRadius = 23.0f;
}

void AISystem::OnPreUpdate(float deltaTime)
{}

void AISystem::OnUpdate(float deltaTime)
{
        CalculateAverage();

        for (auto& aiComp : m_HandleManager->GetActiveComponents<AIComponent>())
        {
                XMVECTOR accel = CalculateAlignment(&aiComp);
                accel += CalculateCohesion(&aiComp);
                accel += CalculateSeperation(&aiComp);

                accel *= aiComp.m_MaxVelocity * deltaTime;

                aiComp.m_Velocity += accel;

                if (MathLibrary::CalulateVectorLength(aiComp.m_Velocity) >
                    MathLibrary::CalulateVectorLength(aiComp.m_MaxVelocity)) // Magnitude
                {
                        XMVector3Normalize(aiComp.m_Velocity);
                        aiComp.m_MaxVelocity *= aiComp.m_MaxVelocity;
                }
        	
        		if (MathLibrary::CalulateVectorLength(aiComp.m_Velocity * aiComp.m_MaxVelocity) >
					MathLibrary::CalulateVectorLength(aiComp.m_MaxVelocity * aiComp.m_MaxVelocity))
                {
                        aiComp.m_Velocity = XMVector3Normalize(aiComp.m_Velocity);
                        aiComp.m_Velocity *= aiComp.m_MaxVelocity;
                }
                aiComp.GetParent().GetComponent<TransformComponent>()->transform.translation += aiComp.m_Velocity * deltaTime;
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
