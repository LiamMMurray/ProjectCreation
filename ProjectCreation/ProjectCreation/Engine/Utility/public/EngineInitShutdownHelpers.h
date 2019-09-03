#pragma once

#include "GEngine.h"
#include <RenderingSystem.h>
#include "../Engine/Audio/AudioManager.h"

namespace EngineHelpers
{
        void InitEngineSystemManagers(RenderSystem::native_handle_type handle);
        void ShutdownEngineSystemManagers();
}
