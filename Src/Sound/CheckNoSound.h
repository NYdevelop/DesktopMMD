#pragma once

#include <vector>

class CheckNoSound
{
private:
    std::vector<bool> judgeHist;
    const float JUDGE_SECOND = 0.3f;

    bool CheckSoundThreshold(double vol);

public:
    bool CheckSound(double vol);
};