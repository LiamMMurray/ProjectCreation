#include "Level_01.h"
#include "../GEngine.h"
#include "../Gameplay/OrbitSystem.h"

#include <DirectXMath.h>

using namespace DirectX;

void Level_01::Enter()
{
        m_SpeedBoostSystem->SetRandomSpawnEnabled(true);
}

void Level_01::Update(float deltaTime)
{
}

void Level_01::Exit()
{}

Level_01::Level_01()
{
        m_LevelType = E_Level_States::LEVEL_01;
        m_SpeedBoostSystem = GEngine::Get()->GetSystemManager()->GetSystem<SpeedBoostSystem>();
}
