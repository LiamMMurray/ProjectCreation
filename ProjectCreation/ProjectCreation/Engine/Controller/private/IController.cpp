#include <IController.h>


void IController::OnUpdate(float deltaTime)
{
        cacheTime = deltaTime;

        GatherInput();
        ProcessInput();
        ApplyInput();
}

void IController::Reset()
{}

void IController::Init(EntityHandle handle)
{
        SetControlledEntity(handle);
}

void IController::Shutdown()
{}
