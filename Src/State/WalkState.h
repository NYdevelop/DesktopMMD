#pragma once

#include "State/StateMMD.h"

#include "MMD/Anim/PlayAnim.h"

#include <memory>
#include "MMD/DrawMMD.h"
#include "State/StateManager.h"

class WalkState : public StateMMD
{
public:
    void Initialize();
    void Doing();
    void End();
    int ModelInitial();

    void SetDrawMMD(std::shared_ptr<DrawMMD> mmd);
    void SetDirection(float direction_rad);


private:
    std::shared_ptr <PlayAnim> walkAnim;
    std::shared_ptr<DrawMMD> m_mmd;

    float m_Direction = 0.f;
    EState nextState = EState::STATE_WAIT;
    const float WALK_SPEED = 0.1f;
};