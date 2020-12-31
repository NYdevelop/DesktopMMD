#include "PlayAnim.h"

#include "DxLib.h"

int PlayAnim::AttachAnime(int modHandle, int animIndex)
{
    // TODO: モーションの動的ロード
    modelHandle = modHandle;
    m_AnimIndex = MV1AttachAnim(modelHandle, animIndex);//モーションの選択
    if (m_AnimIndex == -1)
    {
        MessageBox(NULL, TEXT("MV1AttachAnim error"), NULL, MB_ICONERROR);
        return -1;
    }
    SetMaximumTime(MV1GetAttachAnimTotalTime(modelHandle, m_AnimIndex));
    MV1SetAttachAnimBlendRate(modelHandle, m_AnimIndex, 0);
    return m_AnimIndex;
}

bool PlayAnim::PlayAnimation(float time)
{
    if (playTime < maximumTime)
    {
        playTime += playSpeed;
        if (time != -1)
        {
            playTime = time;
        }
        if (playTime > maximumTime)
        {
            playTime = maximumTime;
        }
    }

    if (playTime >= maximumTime && isLoop)
    {
        ResetAnimTime();
    }

    MV1SetAttachAnimTime(modelHandle, m_AnimIndex, playTime);//モーションの再生位置を設定

    if (playTime >= maximumTime)
    {
        ResetAnimTime();
        return false;
    }
    return true;
}
