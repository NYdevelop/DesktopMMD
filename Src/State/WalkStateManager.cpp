#include "WalkStateManager.h"

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

    // �����ׂ��������Z�o
    UpdateDirection();

    m_StateManager->Transrate(EState::STATE_WALK);
}

void WalkStateManager::Update()
{
    if (m_StateManager->GetCurrentStateIndex() != EState::STATE_WALK)
        return;

    // �ʒu�̃`�F�b�N
    auto pos = m_mmd->GetCharactorPos();

    auto sub = VSub(m_Distination, pos);
    auto distance = sqrt(VDot(sub, sub));

    if (distance < m_Threshold)
    {
        m_StateManager->Transrate(nextState);
    }

    // �i�s�����C��
    UpdateDirection();
}

void WalkStateManager::UpdateDirection()
{
    auto pos = m_mmd->GetCharactorPos();
    auto directVec = VNorm(VSub(m_Distination, pos));
    auto direct_rad = atan2(directVec.x, directVec.z);
    m_Walk->SetDirection(direct_rad);
}
