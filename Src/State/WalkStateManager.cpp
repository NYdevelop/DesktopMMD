#include "WalkStateManager.h"

#include "Util/UtilMMD.h"

void WalkStateManager::Initialize(WalkState* walk, std::shared_ptr<DrawMMD> mmd, std::shared_ptr<StateManager<EState>> stateManager)
{
    m_Walk = walk;
    m_mmd = mmd;
    m_StateManager = stateManager;
}

void WalkStateManager::SetNextState(EState state)
{
    nextState = state;
}


float beforeDirect;
void WalkStateManager::Start(DxLib::VECTOR distination)
{
    m_Distination = distination;

    // 歩くべき向きを算出
    auto direct = CalculateDirection();
    m_mmd->RotateY = direct;
    m_mmd->UpdatePosRot();
    MV1PhysicsResetState(m_mmd->GetModelHandle());

    beforeDirect = direct;

    m_StateManager->Transrate(EState::STATE_WALK);
    m_IsMove = true;
}

void WalkStateManager::Update()
{
    if (m_StateManager->GetCurrentStateIndex() != EState::STATE_WALK || m_IsMove == false)
        return;

    // 進行方向算出
    auto direct = CalculateDirection();
    if (abs(direct - beforeDirect) > DX_PI_F / 2.0f)
    {
        Cancel();
        return;
    }

    m_mmd->RotateY = direct;
    beforeDirect = direct;

    // 進行方向ベクトル
    auto x = sin(direct) * WALK_SPEED;
    auto z = cos(direct) * WALK_SPEED;

    auto pos = m_mmd->GetCharactorPos();
    pos.x += x;
    pos.z += z;
    m_mmd->SetCharactorPos(pos);
}

void WalkStateManager::Cancel()
{
    m_IsMove = false;
    m_StateManager->Transrate(nextState);
}

float WalkStateManager::CalculateDirection()
{
    auto pos = m_mmd->GetCharactorPos();
    auto directVec = VNorm(VSub(m_Distination, pos));
    auto direct_rad = atan2(directVec.x, directVec.z);
    // m_Walk->SetDirection(direct_rad);
    return direct_rad;
}
