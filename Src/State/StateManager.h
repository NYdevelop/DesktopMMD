#pragma once

#include <memory>
#include <map>
#include "State.h"

template <class T>
class StateManager
{
public:
    void AddState(T index, std::shared_ptr<State> state)
    {
        if (m_StateMap.find(index) != m_StateMap.end())
        {
            throw "state add error";
        }

        m_StateMap[index] = state;
    }

    void Transrate(T index)
    {
        if (currentState != nullptr)
        {
            currentState->End();
        }
        if (m_StateMap.find(index) == m_StateMap.end())
        {
            throw "state transrate index error";
        }

        currentState = m_StateMap[index];

        currentState->Initialize();
    }

    void Doing()
    {
        if (currentState == nullptr)
        {
            throw "current state error";
        }
        currentState->Doing();
    }

    void End()
    {
        if (currentState == nullptr) return;
        currentState->End();
    }

    std::map<T, std::shared_ptr<State>> GetStateMap()
    {
        return m_StateMap;
    }

private:
    std::map<T, std::shared_ptr<State>> m_StateMap;
    std::shared_ptr<State> currentState;
};