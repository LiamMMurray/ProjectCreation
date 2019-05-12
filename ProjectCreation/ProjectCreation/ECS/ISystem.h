#pragma once

#include <stdint.h>
#include <type_traits>
#include "ECSTypes.h"

class SystemManager;

class ISystem
{
        friend class SystemManager;

        static TypeId<ISystem> systemTypeID;

    private:
        uint16_t m_Priority;
        float    m_UpdateRateMs;

    protected:
        virtual void OnPreUpdate(float deltaTime)  = 0;
        virtual void OnUpdate(float deltaTime)     = 0;
        virtual void OnPostUpdate(float deltaTime) = 0;
        virtual void OnInitialize()                = 0;
        virtual void OnShutdown()                  = 0;
        virtual void OnResume()                    = 0;
        virtual void OnSuspend()                   = 0;

    public:
        template <typename T>
        TypeId<ISystem> GetTypeID()
        {
                static_assert(std::is_base_of<ISystem, T>::value, "Error. Template type must be subclass of ISystem");

                static const TypeId<ISystem> id{systemTypeID++};
                return id;
        }

		uint16_t GetPriority();
        void     SetPriority(uint16_t priority);
        float    GetUpdateRateMs();
        void     SetUpdateRateMs(float updateRateMs);
};

