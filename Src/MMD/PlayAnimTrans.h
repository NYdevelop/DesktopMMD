#pragma once

#include "AnimInterface.h"

class PlayAnimTrans : public AnimInterface
{
public:
    int AttachAnime(int modHandle, int animIndex);

    void SetSrcAnimIndex(int srcIndex) { srcAnimIndex = srcIndex; }

    /// 移行終了時Falseを返す
    bool PlayAnimation();

    void SetTransTime(int time);

private:
    int modelHandle = 0;
    int srcAnimIndex = 0;
    int transrateAnimIndex = 0;

    int transrateTime = 0;
    int progressTime = 0;
};