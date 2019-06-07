#pragma once
#include <vector>
#include "../../../Utility/Hashing/PairHash.h"
#include "IState.h"

class StateMachine
{
        friend class IState;

    protected:
        std::vector<IState*> m_States;
        IState*              m_Current;

        std::unordered_map<std::pair<IState*, E_STATE_EVENT>, IState*, hash_pair> m_TransitionTable;

        void         Update(float deltaTime);
        virtual void Shutdown();

    public:
        // Call parent constructor
        void Transition(E_STATE_EVENT event);

        template <typename T>
        T* CreateState()
        {
                static_assert(std::is_base_of<IState, T>::value, "Created states must derive from IState");

                T*      state           = new T();
                IState* baseState       = static_cast<IState*>(state);
                baseState->stateMachine = this;
                m_States.push_back(state);

                return state;
        }

        void AddTransition(IState* start, IState* end, E_STATE_EVENT event)
        {
                m_TransitionTable.insert(std::make_pair(std::make_pair(start, event), end));
        }
};