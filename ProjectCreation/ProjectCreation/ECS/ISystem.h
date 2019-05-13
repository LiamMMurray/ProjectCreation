#pragma once

#include <stdint.h>
#include <type_traits>
#include "ECSTypes.h"

class SystemManager;


typedef uint32_t SystemTypeID;

struct FSystemInitProperties
{
        uint32_t m_Flags      = 0;
        float    m_UpdateRate = 0.0f;
        uint16_t m_Priority   = 300;
};

class ISystem
{
        friend class SystemManager;

        static SystemTypeId systemTypeId;

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

        static SystemTypeId GetTypeID()
        {
                static_assert(std::is_base_of<ISystem, T>::value, "Error. Template type must be subclass of ISystem");

                static const SystemTypeId id{systemTypeId++};
                return id;
        }

        uint16_t GetPriority();
        void     SetPriority(uint16_t priority);
        float    GetUpdateRateMs();
        void     SetUpdateRateMs(float updateRateMs);
};
