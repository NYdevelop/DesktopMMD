#pragma once

#include "StateMMD.h"
#include "CaptureSound.h"

#include "PlayAnim.h"
#include "TranrateAnim.h"

#include "EstimateBPM.h"
#include "CalculateBPM.h"
#include "CheckNoSound.h"

#include <memory>

class RhythmState : public StateMMD
{
public:
    void Initialize();
    void Doing();
    void End();

    void OnceInital();

    void ModelInitial();

    inline void SetCapture(std::shared_ptr<CaptureSound> capture)
    {
        m_Capture = move(capture);
    }

    void SetBPM(float bpm);
    void SetNoSound(bool isNoSound);

    void SetMoveAttitude(double rate);

private:
    std::shared_ptr<CaptureSound> m_Capture;

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