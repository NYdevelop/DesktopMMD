#include "TransrateAnim.h"

#include "DxLib.h"

void TransrateAnim::SetAnim(int srcAnim, int transAnim, int modHandle)
{
    srcAnimIndex = srcAnim;
    transrateAnimIndex = transAnim;
    modelHandle = modHandle;
}

void TransrateAnim::SetTransTime(int time)
{
    transrateTime = time;
    progressTime = time;
}

void TransrateAnim::Transrate()
{
    if (progressTime == 0) return;

    if (progressTime > 0) progressTime--;
    auto rate = (float)progressTime / transrateTime;
    MV1SetAttachAnimBlendRate(modelHandle, srcAnimIndex, rate);
    MV1SetAttachAnimBlendRate(modelHandle, transrateAnimIndex, 1 - rate);
}
