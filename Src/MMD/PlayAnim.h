#pragma once

class PlayAnim
{
public:
    void AttachAnime(int modHandle, int animIndex, int AnimSrcMHandle = -1, int NameCheck = 0);

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

    void PlayAnimation();

    inline void ResetAnimTime()
    {
        playTime = 0;
    }

    inline int GetAnimIndex()
    {
        return animeIndex;
    }

private:
    int animeIndex = 0;
    float playTime = 0;
    float playSpeed = 1.0f;
    float maximumTime = 0;
    int modelHandle = 0;
    bool isLoop = true;
};