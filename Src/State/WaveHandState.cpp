#include "WaveHandState.h"

#include <iostream>

using namespace std;

void WaveHandState::Initialize()
{
    cout << "state: wave hand" << endl;

    // ‰æ–Ê•ûŒü‚ðŒü‚­
    auto directVec = VNorm(
        VSub(m_mmd->cameraPos, m_mmd->GetCharactorPos()));
    auto direct_rad = atan2(directVec.x, directVec.z);

    m_mmd->RotateY = direct_rad;
    m_mmd->UpdatePosRot();
    MV1PhysicsResetState(model);

    waveHandLoop->ResetAnimTime();

    animQueue->AddTransrate(-1, waveHandLoop->GetAnimIndex(), 10);
    animQueue->AddAnim(waveHandLoop);
}

void WaveHandState::Doing()
{
    if (animQueue->Empty())
    {
        waveHandLoop->ResetAnimTime();
        animQueue->AddAnim(waveHandLoop);
    }
}

void WaveHandState::End()
{
    animQueue->AddTransrate(waveHandLoop->GetAnimIndex(), -1, 10);
}

int WaveHandState::ModelInitial()
{
    animQueue->SetModel(model);
    waveHandLoop = std::shared_ptr < PlayAnim >(new PlayAnim);
    int ret = waveHandLoop->AttachAnime(model, (int)EAnimIndex::ANIM_WAVE_HAND);
    waveHandLoop->IsLoop(false);

    MV1SetAttachAnimBlendRate(model, waveHandLoop->GetAnimIndex(), 0);

    return ret;
}

void WaveHandState::SetDrawMMD(std::shared_ptr<DrawMMD> mmd)
{
    m_mmd = mmd;
}
