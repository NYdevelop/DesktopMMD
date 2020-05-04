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
    if (animQueue->Empty())
    {
        walkAnim->ResetAnimTime();
        animQueue->AddAnim(walkAnim);
        return;
    }

    m_mmd->RotateY = m_Direction;
    // 進行方向ベクトル
    auto x = sin(m_Direction) * WALK_SPEED;
    auto z = cos(m_Direction) * WALK_SPEED;

    auto pos = m_mmd->GetCharactorPos();
    pos.x += x;
    pos.z += z;
    m_mmd->SetCharactorPos(pos);
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
