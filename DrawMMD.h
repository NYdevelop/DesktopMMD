#pragma once

#include "DxLib.h"

#include "PlayAnim.h"
#include "TranrateAnim.h"

class DrawMMD
{
public:
    void preInitialize();
    void afterInitialize();
    int mainProcess();
    void SetBPM(float bpm);
    void SetNoSound(bool isNoSound);

    void SetMoveAttitude(double rate);

private:
    int model;//モデルハンドル
    DxLib::VECTOR charaPos;//キャラクタの座標

    PlayAnim unazuki;
    PlayAnim blink;
    int stableAnimIndex = -1;

    TransrateAnim unazuki2NoSound;
    TransrateAnim noSound2Unazuki;
    TransrateAnim noSound2Stable;

    LONG dispWidth;
    LONG dispHeight;

    float m_Bpm;
    bool m_IsNoSound = false;

    const float BASE_BPM = 46.8f;
    const float BASE_PLAY_SPEED = 0.5f;
};