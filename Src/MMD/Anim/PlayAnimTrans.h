#pragma once

#include "AnimInterface.h"

class PlayAnimTrans : public AnimInterface
{
public:
    int AttachAnime(int modHandle, int animIndex);

    /// ˆÚsI—¹False‚ğ•Ô‚·
    bool PlayAnimation(float time = -1);

    void SetTransTime(int time);

    int srcAnimIndex = 0;
    int transrateAnimIndex = 0;

private:
    int modelHandle = 0;


    int transrateTime = 0;
    int progressTime = 0;
};