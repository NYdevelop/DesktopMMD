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
    int model;//モデルハンドル
    int attachIndex;//モデルに適用するアニメーションの番号
    float playTime;//アニメーションの現在時刻
    float totalTime;//アニメーションの総再生時間
    DxLib::VECTOR charaPos;//キャラクタの座標

    LONG dispWidth;
    LONG dispHeight;

    float m_Bpm;

    float playSpeed;

    const float BASE_BPM = 51.2f;
    const float BASE_PLAY_SPEED = 0.5f;
};