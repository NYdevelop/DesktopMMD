#pragma once

#include "DxLib.h"

class DrawMMD
{
public:
    void preInitialize();
    void afterInitialize();
    int mainProcess();
    void SetBPM(float bpm);

private:
    int model;//���f���n���h��
    int attachIndex;//���f���ɓK�p����A�j���[�V�����̔ԍ�
    float playTime;//�A�j���[�V�����̌��ݎ���
    float totalTime;//�A�j���[�V�����̑��Đ�����
    DxLib::VECTOR charaPos;//�L�����N�^�̍��W

    LONG dispWidth;
    LONG dispHeight;

    float m_Bpm;

    float playSpeed;

    const float BASE_BPM = 51.2f;
    const float BASE_PLAY_SPEED = 0.5f;
};