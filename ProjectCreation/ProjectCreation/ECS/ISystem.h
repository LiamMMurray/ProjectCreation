#pragma once

#include <stdint.h>
#include <type_traits>
#include "ECSTypes.h"

class SystemManager;

#define SYSTEM_INIT_FLAG_SUSPEND_ON_START 1 << 1
#define SYSTEM_FLAG_UPDATE_WHEN_PAUSED 1 << 2

typedef uint32_t SystemTypeID;

struct FSystemProperties
{
        uint32_t m_Flags      = 0;
        float    m_UpdateRate = 0.0f;
        uint16_t m_Priority   = 300;
};

enum E_SYSTEM_PRIORITY
{
        VERY_LOW  = 100,
        LOW       = 200,
        NORMAL    = 300,
        HIGH      = 400,
        VERY_HIGH = 500
};

class ISystem
{
        friend class SystemManager;
        friend struct PriorityComparator;

        static SystemTypeId systemTypeId;

    private:
        FSystemProperties m_Properties;

    protected:
        virtual void OnPreUpdate(float deltaTime)  = 0;
        virtual void OnUpdate(float deltaTime)     = 0;
        virtual void OnPostUpdate(float deltaTime) = 0;
        virtual void OnInitialize()                = 0;
        virtual void OnShutdown()                  = 0;
        virtual void OnResume()                    = 0;
        virtual void OnSuspend()                   = 0;

    public:
        virtual ~ISystem()
        {} // = default;

        template <typename T>
        static SystemTypeId GetTypeID()
        {
                static_assert(std::is_base_of<ISystem, T>::value, "Error. Template type must be subclass of ISystem");

                static const SystemTypeId id{systemTypeId++};
                return id;
        }

        inline const FSystemProperties& GetSystemProperties() const
        {
                return m_Properties;
        }

        inline void SetSystemProperties(const FSystemProperties& val)
        {
                m_Properties = val;
        }
};
