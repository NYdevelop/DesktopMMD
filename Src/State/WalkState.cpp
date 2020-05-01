#include "WalkState.h"

#include <iostream>
#include "DxLib.h"

#include "Util/WinUtil.h"

#define M_PI       3.14159265358979323846f

using namespace std;

void WalkState::Initialize()
{
    cout << "state: walk" << endl;

    m_mmd->RotateY = m_Direction;
    m_mmd->UpdatePosRot();
    MV1SetAttachAnimBlendRate(model, walkAnim.GetAnimIndex(), 1);
    walkAnim.ResetAnimTime();
    MV1PhysicsResetState(model);
}

void WalkState::Doing()
{
    walkAnim.PlayAnimation();

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
    MV1SetAttachAnimBlendRate(model, walkAnim.GetAnimIndex(), 0);
    MV1PhysicsResetState(model);
}

void WalkState::ModelInitial()
{
    walkAnim.AttachAnime(model, 9);
    walkAnim.SetPlaySpeed(1.0f);
    MV1SetAttachAnimBlendRate(model, walkAnim.GetAnimIndex(), 0);
}

void WalkState::SetDrawMMD(std::shared_ptr<DrawMMD> mmd)
{
    m_mmd = mmd;
}

void WalkState::SetDirection(float direction_rad)
{
    m_Direction = direction_rad;
}
