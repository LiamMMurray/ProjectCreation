#pragma once

#include <GEngine.h>
#include <RenderingSystem.h>
#include <AudioManager.h>

namespace EngineUtil
{
        void InitEngineSystemManagers(RenderSystem::native_handle_type handle);
        void ShutdownEngineSystemManagers();
}
