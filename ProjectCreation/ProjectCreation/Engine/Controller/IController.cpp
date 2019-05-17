#include "IController.h"

void IController::OnUpdate(float deltaTime)
{
        GatherInput();
        ProcessInput();
        ApplyInput();
}
