#pragma once

#include "AnimInterface.h"

class PlayAnim : public AnimInterface
{
public:
    int AttachAnime(int modHandle, int animIndex);

    inline void SetMaximumTime(float time)
    {
        maximumTime = time;
    }

    inline void SetPlaySpeed(float value)
    {
        playSpeed = value;
    }

    inline void IsLoop(bool flag)
    {
        isLoop = flag;
    }

    /// 最後まで再生したらFalseを返す
    bool PlayAnimation(float time = -1);

    inline void ResetAnimTime()
    {
        playTime = 0.f;
    }

private:
    float playTime = 0;
    float playSpeed = 1.0f;
    float maximumTime = 0;
    int modelHandle = 0;
    bool isLoop = true;
};