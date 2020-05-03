#pragma once

#include "State.h"
#include "MMD/PlayAnimQueue.h"
#include "StateManager.h"

class StateMMD : public State
{
public:
    inline void SetModel(int modelHandle)
    {
        model = modelHandle;
    }

    inline void SetAnimQueue(std::shared_ptr<PlayAnimQueue> queue)
    {
        animQueue = queue;
    }

    inline void SetStateManager(StateManager<EState>* manager, std::map<EState, std::vector<int>>* animMap)
    {
        stateManager = manager;
        animationMap = animMap;
    }

    virtual int ModelInitial() = 0;

protected:
    int model = 0;
    std::shared_ptr<PlayAnimQueue> animQueue;
    StateManager<EState>* stateManager = nullptr;
    std::map<EState, std::vector<int>>* animationMap;
};