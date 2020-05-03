#include "rhythmState.h"

#include "DxLib.h"

#include <iostream>

using namespace std;

void RhythmState::Initialize()
{
    cout << "state: rhythm" << endl;

    cout << BASE_PLAY_SPEED << endl;
    m_Bpm = BASE_BPM;

    m_Capture->SetCaptureCallback([this](WAVEHDR wh)
    {
        auto wf = m_Capture->GetWaveFormatEx();
        auto currentVolume = calcBPM.CalcVolume(wh, wf);
        auto bpms = calcBPM.CalcBPM(currentVolume, wf);
        if (bpms.size() == 0) return;
        auto ret = estimate.Estimate(bpms);
        SetBPM(ret);
    });

    m_Capture->Start();
}

void RhythmState::Doing()
{
    // なめらかなアニメーション変化
    unazuki2NoSound.PlayAnimation();
    noSound2Unazuki.PlayAnimation();
    noSound2Stable.PlayAnimation();

    if (m_IsNoSound == false)
    {
        unazuki.PlayAnimation();
    }
}

void RhythmState::End()
{
    m_Capture->Stop();

    m_Capture->SetCaptureCallback(nullptr);

    MV1SetAttachAnimBlendRate(model, noSoundAnimIndex, 0);
    MV1SetAttachAnimBlendRate(model, stableAnimIndex, 0);
    MV1SetAttachAnimBlendRate(model, unazuki.GetAnimIndex(), 0);
}

void RhythmState::OnceInital()
{
    calcBPM.SetCalcVolume([this](double volume)
    {
        /// 動きの大きさ調整
        auto rate = (float)volume / 1000.f;
        if (rate > 1)
        {
            rate = 1;
        }
        SetMoveAttitude(rate);

        /// 鳴っているかどうか
        bool isNoSound = checker.CheckSound(volume);
        SetNoSound(isNoSound);
        if (isNoSound == true)
        {
            calcBPM.Clear();
            cout << "clear" << endl;
            return;
        }
    });
}

int RhythmState::ModelInitial()
{
    unazuki.AttachAnime(model, (int)EAnimIndex::ANIM_UNAZUKI);
    unazuki.SetPlaySpeed(BASE_PLAY_SPEED);

    noSoundAnimIndex = (int)EAnimIndex::ANIM_NO_SOUND;
    noSoundAnimIndex = MV1AttachAnim(model, noSoundAnimIndex, -1, FALSE);//モーションの選択

    stableAnimIndex = (int)EAnimIndex::ANIM_UNAZUKI_STABLE;
    stableAnimIndex = MV1AttachAnim(model, stableAnimIndex, -1, FALSE);//モーションの選択

    unazuki2NoSound.srcAnimIndex = unazuki.GetAnimIndex();
    unazuki2NoSound.AttachAnime(model, noSoundAnimIndex);
    noSound2Unazuki.srcAnimIndex = noSoundAnimIndex;
    noSound2Unazuki.AttachAnime(model, unazuki.GetAnimIndex());
    noSound2Stable.srcAnimIndex = noSoundAnimIndex;
    noSound2Stable.AttachAnime(model, stableAnimIndex);

    MV1SetAttachAnimTime(model, noSoundAnimIndex, 0);//モーションの再生位置を設定
    MV1SetAttachAnimBlendRate(model, noSoundAnimIndex, 0);

    MV1SetAttachAnimTime(model, stableAnimIndex, 0);//モーションの再生位置を設定
    MV1SetAttachAnimBlendRate(model, stableAnimIndex, 0);

    return 0;
}

void RhythmState::SetBPM(float bpm)
{
    cout << bpm << ": ";
    if (bpm > 140) bpm /= 2.0f;
    cout << bpm << endl;
    unazuki.SetPlaySpeed(BASE_PLAY_SPEED * bpm / BASE_BPM);
    m_Bpm = bpm;
}

void RhythmState::SetNoSound(bool isNoSound)
{
    if (m_IsNoSound == isNoSound) return;

    if (isNoSound == true)
    {
        unazuki2NoSound.SetTransTime(20);
    }
    else
    {
        noSound2Unazuki.SetTransTime(10);
        noSound2Stable.SetTransTime(10);
    }
    m_IsNoSound = isNoSound;
}

void RhythmState::SetMoveAttitude(float rate)
{
    MV1SetAttachAnimBlendRate(model, unazuki.GetAnimIndex(), rate);
    MV1SetAttachAnimBlendRate(model, stableAnimIndex, 1 - rate);
}
