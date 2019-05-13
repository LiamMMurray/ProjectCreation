#include "ResourceManager.h"
#include <assert.h>

CResourceManager* CResourceManager::instance;

void CResourceManager::Initialize()
{
        assert(instance == nullptr);

        instance = new CResourceManager();
}

void CResourceManager::Shutdown()
{
        assert(instance != nullptr);

        delete instance;
}

CResourceManager* CResourceManager::Get()
{
        assert(instance != nullptr);

		return instance;
}
