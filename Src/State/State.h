#pragma once

class State
{
public:
    virtual void Initialize() = 0;
    virtual void Doing() = 0;
    virtual void End() = 0;
};