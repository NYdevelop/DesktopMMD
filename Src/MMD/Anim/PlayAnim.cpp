#include "PlayAnim.h"

#include "DxLib.h"

int PlayAnim::AttachAnime(int modHandle, int animIndex)
{
    // TODO: ���[�V�����̓��I���[�h
    modelHandle = modHandle;
    m_AnimIndex = MV1AttachAnim(modelHandle, animIndex);//���[�V�����̑I��
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

    MV1SetAttachAnimTime(modelHandle, m_AnimIndex, playTime);//���[�V�����̍Đ��ʒu��ݒ�

    if (playTime >= maximumTime)
    {
        ResetAnimTime();
        return false;
    }
    return true;
}
