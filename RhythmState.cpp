#include "rhythmState.h"


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
    // �Ȃ߂炩�ȃA�j���[�V�����ω�
    unazuki2NoSound.Transrate();
    noSound2Unazuki.Transrate();
    noSound2Stable.Transrate();

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
    model = m_mmd->GetModelHandle();

    unazuki.AttachAnime(model, 3);
    unazuki.SetPlaySpeed(BASE_PLAY_SPEED);

    noSoundAnimIndex = 1;
    noSoundAnimIndex = MV1AttachAnim(model, noSoundAnimIndex, -1, FALSE);//���[�V�����̑I��

    stableAnimIndex = 4;
    stableAnimIndex = MV1AttachAnim(model, stableAnimIndex, -1, FALSE);//���[�V�����̑I��

    unazuki2NoSound.SetAnim(unazuki.GetAnimIndex(), noSoundAnimIndex, model);
    noSound2Unazuki.SetAnim(noSoundAnimIndex, unazuki.GetAnimIndex(), model);
    noSound2Stable.SetAnim(noSoundAnimIndex, stableAnimIndex, model);

    MV1SetAttachAnimTime(model, noSoundAnimIndex, 0);//���[�V�����̍Đ��ʒu��ݒ�
    MV1SetAttachAnimBlendRate(model, noSoundAnimIndex, 0);

    MV1SetAttachAnimTime(model, stableAnimIndex, 0);//���[�V�����̍Đ��ʒu��ݒ�
    MV1SetAttachAnimBlendRate(model, stableAnimIndex, 0);

    calcBPM.SetCalcVolume([this](double volume)
    {
        /// �����̑傫������
        auto rate = (float)volume / 1000.f;
        if (rate > 1)
        {
            rate = 1;
        }
        SetMoveAttitude(rate);

        /// ���Ă��邩�ǂ���
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

void RhythmState::SetMoveAttitude(double rate)
{
    MV1SetAttachAnimBlendRate(model, unazuki.GetAnimIndex(), rate);
    MV1SetAttachAnimBlendRate(model, stableAnimIndex, 1 - rate);
}