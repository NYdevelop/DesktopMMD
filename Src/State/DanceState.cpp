#include "DanceState.h"

#include <iostream>
#include "DxLib.h"
#include <chrono>

using namespace std;

void DanceState::Initialize()
{
    cout << "state: dance" << endl;

    // モデル位置リセット
    auto pos = m_mmd->GetCharactorPos();
    pos.x = 0.f;
    pos.y = -7.f;
    m_mmd->SetCharactorPos(pos);
    m_mmd->RotateY = 3.14f;
    m_mmd->SetZoom(35.f);
    m_mmd->UpdatePosRot();

    MV1SetAttachAnimBlendRate(model, danceAnim->GetAnimIndex(), 1);
    danceAnim->ResetAnimTime();
    animQueue->AddAnim(danceAnim);
    MV1PhysicsResetState(model);

    m_Output->Output(L"data/music.wav");
}


void DanceState::Doing()
{
}

void DanceState::End()
{
    m_Output->Stop();
    m_Output->CloseDevice();

    animQueue->SetCurrentStop();
    animQueue->AddTransrate(danceAnim->GetAnimIndex(), -1, 10);
}

int DanceState::ModelInitial()
{
    danceAnim = std::shared_ptr<PlayAnim>(new PlayAnim);
    int ret = danceAnim->AttachAnime(model, (int)EAnimIndex::ANIM_DANCE);
    danceAnim->IsLoop(false);
    danceAnim->SetPlaySpeed(.575f);
    MV1SetAttachAnimBlendRate(model, danceAnim->GetAnimIndex(), 0);
    return ret;
}

void DanceState::SetOutputSound(std::shared_ptr<OutputSound> output)
{
    m_Output = output;
}

void DanceState::SetDrawMMD(std::shared_ptr<DrawMMD> mmd)
{
    m_mmd = mmd;
}
