#include "WalkState.h"

#include <iostream>
#include "DxLib.h"

#include "Util/WinUtil.h"
#include "Define.h"

using namespace std;

void WalkState::Initialize()
{
    cout << "state: walk" << endl;

    m_mmd->canViewCamera = true;

    m_mmd->RotateY = m_Direction;
    m_mmd->UpdatePosRot();
    MV1PhysicsResetState(model);

    walkAnim->ResetAnimTime();

    animQueue->AddTransrate(-1, walkAnim->GetAnimIndex(), 10);
    animQueue->AddAnim(walkAnim);
}

void WalkState::Doing()
{
    // Žè‚ðU‚ç‚È‚¢‚Ì‚Í‚»‚¤‚¢‚¤ƒ‚[ƒVƒ‡ƒ“‚Å‚ ‚é‚½‚ß
    if (animQueue->Empty())
    {
        walkAnim->ResetAnimTime();
        animQueue->AddAnim(walkAnim);
        return;
    }
}

void WalkState::End()
{
    m_mmd->canViewCamera = false;
    MV1SetAttachAnimBlendRate(model, walkAnim->GetAnimIndex(), 0);
    MV1PhysicsResetState(model);
}

int WalkState::ModelInitial()
{
    walkAnim = shared_ptr<PlayAnim>(new PlayAnim());
    auto ret = walkAnim->AttachAnime(model, (int)EAnimIndex::ANIM_WALK);
    walkAnim->SetPlaySpeed(1.0f);
    walkAnim->IsLoop(false);
    MV1SetAttachAnimBlendRate(model, walkAnim->GetAnimIndex(), 0);
    return ret;
}

void WalkState::SetDrawMMD(std::shared_ptr<DrawMMD> mmd)
{
    m_mmd = mmd;
}

void WalkState::SetDirection(float direction_rad)
{
    m_Direction = direction_rad;
}
