#include "PlayAnimTrans.h"

#include "DxLib.h"

int PlayAnimTrans::AttachAnime(int modHandle, int animIndex)
{
    modelHandle = modHandle;
    srcAnimIndex = srcAnimIndex;
    transrateAnimIndex = animIndex;
    return 0;
}

bool PlayAnimTrans::PlayAnimation(float time)
{
    if (progressTime == 0) return false;

    if (progressTime > 0) progressTime--;
    auto rate = (float)progressTime / transrateTime;
    if (srcAnimIndex != -1)
    {
        MV1SetAttachAnimBlendRate(modelHandle, srcAnimIndex, rate);
    }
    if (transrateAnimIndex != -1)
    {
        MV1SetAttachAnimBlendRate(modelHandle, transrateAnimIndex, 1 - rate);
    }
    if (progressTime == 0)
    {
        MV1PhysicsResetState(modelHandle);
        return false;
    }
    return true;
}

void PlayAnimTrans::SetTransTime(int time)
{
    transrateTime = time;
    progressTime = time;
}
