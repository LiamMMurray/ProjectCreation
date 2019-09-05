#pragma once
#include <PlayerControllerState.h>

#include <MathLibrary.h>
// Audio Includes
#include <AudioManager.h>
#include <Interface/G_Audio/GMusic.h>
#include <Interface/G_Audio/GSound.h>

class PlayerEndState : public IPlayerControllerState
{
    private:
       
		float m_Timer;
        bool  m_GameEnded;
    public:
        // Inherited via IPlayerControllerState
        virtual void Enter() override;
        virtual void Update(float deltaTime) override;
        virtual void Exit() override;     
};