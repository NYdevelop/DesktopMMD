#pragma once

#include "StateMMD.h"

#include <iostream>

class WaitState : public StateMMD
{
public:
    inline void Initialize() { std::cout << "state: wait" << std:: endl; }
    inline void Doing() {}
    inline void End() {}
    inline void ModelInitial() {};
};