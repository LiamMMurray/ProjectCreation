#include "Level_02.h"
#include "../GEngine.h"

void Level_02::Enter()
{
        GEngine::Get()->SetDesiredPlayerRadius(1500.0f);
        GEngine::Get()->SetTransitionSpeed(15.0f);
        GEngine::Get()->Update();
}

void Level_02::Update(float deltaTime)
{
}

void Level_02::Exit()
{}

Level_02::Level_02()
{
        m_LevelType = E_Level_States::LEVEL_02;
}
