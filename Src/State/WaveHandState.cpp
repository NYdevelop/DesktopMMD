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

    waveHandLoop->ResetAnimTime();

    // auto trans = shared_ptr<PlayAnimTrans>(new PlayAnimTrans);
    // trans->AttachAnime();

    transrateTime = 20;
    progressTime = transrateTime;
    //anim.AddAnim(trans); // ˆÚsŠúŠÔ

    anim.AddAnim(waveHandLoop);
    MV1PhysicsResetState(model);
}

void WaveHandState::Doing()
{
    if (progressTime-- > 0)
    {
        auto rate = (float)progressTime / transrateTime;
        MV1SetAttachAnimBlendRate(
            model, waveHandLoop->GetAnimIndex(), 1 - rate);
        return;
    }

    if (anim.Play() == false)
    {
        anim.AddAnim(waveHandLoop);
    }
}

void WaveHandState::End()
{
    anim.Clear();
    MV1PhysicsResetState(model);
}

void WaveHandState::ModelInitial()
{
    anim.SetModel(model);
    waveHandLoop = std::shared_ptr < PlayAnim >(new PlayAnim);
    waveHandLoop->AttachAnime(model, 10);

    MV1SetAttachAnimBlendRate(
        model, waveHandLoop->GetAnimIndex(), 0);
}

void WaveHandState::SetDrawMMD(std::shared_ptr<DrawMMD> mmd)
{
    m_mmd = mmd;
}
