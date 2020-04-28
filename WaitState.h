#pragma once

#include "State.h"

#include <iostream>

class WaitState : public State
{
public:
    inline void Initialize() { std::cout << "state: wait" << std:: endl; }
    inline void Doing() {}
    inline void End() {}
};