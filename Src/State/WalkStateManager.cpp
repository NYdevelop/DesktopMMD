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


void WalkStateManager::Start(DxLib::VECTOR distination)
{
    m_Distination = distination;

    // 歩くべき向きを算出
    UpdateDirection();

    m_StateManager->Transrate(EState::STATE_WALK);
    m_IsMove = true;
}

void WalkStateManager::Update()
{
    if (m_StateManager->GetCurrentStateIndex() != EState::STATE_WALK || m_IsMove == false)
        return;

    // 位置のチェック
    auto pos = m_mmd->GetCharactorPos();
    auto distance = GetDistance(m_Distination, pos);
    if (distance < m_Threshold)
    {
        Cancel();
        return;
    }

    // 進行方向修正
    auto direct = UpdateDirection();

    // 進行方向ベクトル
    auto x = sin(direct) * WALK_SPEED;
    auto z = cos(direct) * WALK_SPEED;

    pos.x += x;
    pos.z += z;
    m_mmd->SetCharactorPos(pos);
}

void WalkStateManager::Cancel()
{
    m_IsMove = false;
    m_StateManager->Transrate(nextState);
}

float WalkStateManager::UpdateDirection()
{
    auto pos = m_mmd->GetCharactorPos();
    auto directVec = VNorm(VSub(m_Distination, pos));
    auto direct_rad = atan2(directVec.x, directVec.z);
    // m_Walk->SetDirection(direct_rad);
    m_mmd->RotateY = direct_rad;
    return direct_rad;
}
