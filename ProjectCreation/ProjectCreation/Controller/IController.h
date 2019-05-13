#pragma once

class IController
{
    private:
        virtual void GatherInput()  = 0;
        virtual void ProcessInput() = 0;
        virtual void ApplyInput()   = 0;

    public:
        virtual void OnUpdate(float deltaTime);
};
