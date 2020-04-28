#pragma once

#include "State.h"
#include "CaptureSound.h"
#include "DrawMMD.h"

#include "PlayAnim.h"
#include "TranrateAnim.h"

#include "EstimateBPM.h"
#include "CalculateBPM.h"
#include "CheckNoSound.h"

#include <memory>

class RhythmState : public State
{
public:
    void Initialize();
    void Doing();
    void End();

    void OnceInital();

    inline void SetCapture(std::shared_ptr<CaptureSound> capture)
    {
        m_Capture = move(capture);
    }

    inline void SetMMD(std::shared_ptr<DrawMMD> mmd)
    {
        m_mmd = move(mmd);
    }

    void SetBPM(float bpm);
    void SetNoSound(bool isNoSound);

    void SetMoveAttitude(double rate);

private:
    std::shared_ptr<CaptureSound> m_Capture;
    std::shared_ptr<DrawMMD> m_mmd;

    int model;

    PlayAnim unazuki;
    int stableAnimIndex = -1;
    int noSoundAnimIndex = -1;

    TransrateAnim unazuki2NoSound;
    TransrateAnim noSound2Unazuki;
    TransrateAnim noSound2Stable;

    bool m_IsNoSound = false;

    float m_Bpm;

    const float BASE_BPM = 46.8f;
    const float BASE_PLAY_SPEED = 0.5f;

    CalculateBPM calcBPM;
    CheckNoSound checker;
    EstimateBPM estimate;
};