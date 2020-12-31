#pragma once

#include <memory>
#include <map>
#include "State.h"

#include "Util/WinUtil.h"
#include <iostream>

//! T: State‚ðŽw’è‚·‚éIndex‚ÌŒ^
template <class T>
class StateManager
{
public:
    void Initialize(T index)
    {
        previousStateIndex = index;
        currentStateIndex = index;
        SetState(index);
    }

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
        isInitialize = false;
        if (m_StateMap.find(index) == m_StateMap.end())
        {
            throw "state transrate index error";
        }

        if (currentState != nullptr)
        {
            currentState->End();
        }
        previousStateIndex = currentStateIndex;
        currentState = m_StateMap[index];
        currentStateIndex = index;

        currentState->Initialize();
        isInitialize = true;
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
        if (isInitialize == false) return;
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

    T GetPreviousStateIndex()
    {
        return previousStateIndex;
    }

private:
    std::map<T, std::shared_ptr<State>> m_StateMap;
    std::shared_ptr<State> currentState;
    T currentStateIndex;
    T previousStateIndex;
    bool isInitialize = false;
};