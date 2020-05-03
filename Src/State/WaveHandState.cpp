#include "WaveHandState.h"

#include <iostream>

#include "MMD/PlayAnimTrans.h"

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

    auto trans = shared_ptr<PlayAnimTrans>(new PlayAnimTrans);
    auto animVec = (*animationMap)[stateManager->GetPreviousStateIndex()];
    if (animVec.size() != 0)
    {
        trans->SetSrcAnimIndex((int)animVec[0]);
    }
    trans->AttachAnime(model, waveHandLoop->GetAnimIndex());
    trans->SetTransTime(10);
    animQueue->AddAnim(trans);

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
