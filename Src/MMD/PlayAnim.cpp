#include "PlayAnim.h"

#include "DxLib.h"

void PlayAnim::AttachAnime(int modHandle, int animIndex, int AnimSrcMHandle, int NameCheck)
{
    // TODO: モーションの動的ロード
    modelHandle = modHandle;
    animeIndex = MV1AttachAnim(modelHandle, animIndex, AnimSrcMHandle, NameCheck);//モーションの選択
    if (animeIndex == -1)
    {
        MessageBox(NULL, TEXT("MV1AttachAnim error"), NULL, MB_ICONERROR);
    }
    SetMaximumTime(MV1GetAttachAnimTotalTime(modelHandle, animeIndex));
}

bool PlayAnim::PlayAnimation()
{
    if (playTime < maximumTime)
    {
        playTime += playSpeed;
        if (playTime > maximumTime)
        {
            playTime = maximumTime;
        }
    }

    if (playTime >= maximumTime && isLoop)
    {
        playTime = 0.f;
    }

    MV1SetAttachAnimTime(modelHandle, animeIndex, playTime);//モーションの再生位置を設定

    if (playTime >= maximumTime)
    {
        return false;
    }
    return true;
}
