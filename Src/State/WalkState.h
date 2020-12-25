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

private:
    std::shared_ptr <PlayAnim> walkAnim;
    std::shared_ptr<DrawMMD> m_mmd;

    EState nextState = EState::STATE_WAIT;
};