#pragma once

#include "State.h"
#include "MMD/PlayAnimQueue.h"

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

    virtual int ModelInitial() = 0;

protected:
    int model = 0;
    std::shared_ptr<PlayAnimQueue> animQueue;
};