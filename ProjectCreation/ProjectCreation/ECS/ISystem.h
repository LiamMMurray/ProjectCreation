#pragma once
class ISystem
{
    public:
        void OnPreUpdate(float deltaTime);
        void OnUpdate(float deltaTime);
        void OnPostUpdate(float deltaTime);
        void OnInitialize();
        void OnShutdown();
        void OnResume();
        void OnSuspend();
};