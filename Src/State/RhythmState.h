#pragma once

#include "State/StateMMD.h"
#include "Sound/CaptureSound.h"

#include "MMD/Anim/PlayAnim.h"
#include "MMD/Anim/PlayAnimTrans.h"

#include "Sound/EstimateBPM.h"
#include "Sound/CalculateBPM.h"
#include "Sound/CheckNoSound.h"

#include <memory>

class RhythmState : public StateMMD
{
public:
    void Initialize();
    void Doing();
    void End();

    void OnceInital();

    int ModelInitial();

    inline void SetCapture(std::shared_ptr<CaptureSound> capture)
    {
        m_Capture = move(capture);
    }

    void SetBPM(float bpm);
    void SetNoSound(bool isNoSound);

    void SetMoveAttitude(float rate);

private:
    std::shared_ptr<CaptureSound> m_Capture;

    PlayAnim unazuki;
    int stableAnimIndex = -1;
    int noSoundAnimIndex = -1;

    PlayAnimTrans unazuki2NoSound;
    PlayAnimTrans noSound2Unazuki;
    PlayAnimTrans noSound2Stable;

    bool m_IsNoSound = false;

    float m_Bpm = 0;

    const float BASE_BPM = 46.8f;
    const float BASE_PLAY_SPEED = 0.5f;

    CalculateBPM calcBPM;
    CheckNoSound checker;
    EstimateBPM estimate;
};