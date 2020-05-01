#pragma once

#include <memory>
#include <map>
#include "State.h"

#include "Util/WinUtil.h"
#include <iostream>

//! T: State‚ğw’è‚·‚éIndex‚ÌŒ^
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

    void SetState(T index)
    {
        if (m_StateMap.find(index) == m_StateMap.end())
        {
            throw "state transrate index error";
        }

        if (currentState != nullptr)
        {
            currentState->End();
        }
        currentState = m_StateMap[index];
        currentStateIndex = index;

        currentState->Initialize();
    }

    void Transrate(T index)
    {
        if (currentStateIndex == index)
        {
            std::cout << "already state: " << as_integer(index) << std::endl;
            return;
        }
        SetState(index);
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

    T GetCurrentStateIndex()
    {
        return currentStateIndex;
    }

private:
    std::map<T, std::shared_ptr<State>> m_StateMap;
    std::shared_ptr<State> currentState;
    T currentStateIndex;
};