#pragma once

class CResourceManager
{
        static CResourceManager* instance;

    public:
        static void Initialize();
        static void Shutdown();
        static CResourceManager* Get();

};