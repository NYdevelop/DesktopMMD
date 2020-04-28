#include "CheckNoSound.h"

#include <algorithm>

bool CheckNoSound::CheckSoundThreshold(double vol)
{
    if (vol < 5)
    {
        return true;
    }
    return false;
}

bool CheckNoSound::CheckSound(double vol)
{
    judgeHist.emplace_back(CheckSoundThreshold(vol));
    if (judgeHist.size() > JUDGE_SECOND * 10)
    {
        judgeHist.erase(judgeHist.begin());
    }

    if (std::find_if(judgeHist.begin(), judgeHist.end(), [](bool x) { return x == false; }) != judgeHist.end())
    {
        return false;
    }
    return true;
}
