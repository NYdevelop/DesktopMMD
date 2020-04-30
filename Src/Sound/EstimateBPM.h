#pragma once

#include <vector>

class EstimateBPM
{
public:
    float Estimate(std::vector<int> val);
    inline float GetValue() { return previousValue; }

private:
    float previousValue = 0;
};