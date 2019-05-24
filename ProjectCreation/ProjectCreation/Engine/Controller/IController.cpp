#include "IController.h"

void IController::OnUpdate(float deltaTime)
{
        cacheTime = deltaTime;
        GatherInput();
        ProcessInput();
        ApplyInput();
}

void IController::Init(EntityHandle handle)
{
        SetControlledEntity(handle);
}
