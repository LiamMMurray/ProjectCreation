#include "IController.h"

void IController::OnUpdate(float deltaTime)
{
        GatherInput();
        ProcessInput();
        ApplyInput();
}

void IController::Init(EntityHandle handle)
{
        SetControlledEntity(handle);
}
