#pragma once

#include "Define.h"
#include <memory>

#include "State/StateManager.h"
#include "State/WalkState.h"
#include "MMD/DrawMMD.h"

class WalkStateManager
{
public:
    void Initialize(WalkState* walk, std::shared_ptr<DrawMMD> mmd, std::shared_ptr<StateManager<EState>> stateManager);
    void SetNextState(EState state);

    void Start(DxLib::VECTOR distination);
    void Update();

    bool IsMove() { return m_IsMove; };

    void Cancel();

private:
    float CalculateDirection();

    DxLib::VECTOR m_Distination = VGet(0,0,0);
    EState nextState = EState::STATE_WAIT;

    std::shared_ptr<StateManager<EState>> m_StateManager;
    std::shared_ptr<DrawMMD> m_mmd;
    WalkState* m_Walk = nullptr;

    bool m_IsMove = false;
    const float WALK_SPEED = 0.1f;
};