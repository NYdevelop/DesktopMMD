#pragma once

#include "State.h"

class StateMMD : public State
{
public:
    inline void SetModel(int modelHandle)
    {
        model = modelHandle;
    }

    virtual void ModelInitial() = 0;

protected:
    int model = 0;
};