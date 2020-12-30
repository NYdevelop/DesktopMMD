#pragma once

#include "State.h"
#include "MMD/Anim/ActionManager.h"

class StateMMD : public State
{
public:
    inline void SetModel(int modelHandle)
    {
        model = modelHandle;
    }

    inline void SetAnimManager(const std::shared_ptr<ActionManager>& manage)
    {
        animManager = manage;
    }

    virtual int ModelInitial() = 0;

protected:
    int model = 0;
    std::shared_ptr<ActionManager> animManager;
};