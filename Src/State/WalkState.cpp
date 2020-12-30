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

    walkAnim->ResetAnimTime();

    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->AddTransrate(-1, walkAnim->GetAnimIndex(), 10);
    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->AddAnim(walkAnim);
}

void WalkState::Doing()
{
    if (animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->Empty())
    {
        walkAnim->ResetAnimTime();
        animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->AddAnim(walkAnim);
        return;
    }
}

void WalkState::End()
{
    m_mmd->canViewCamera = false;
    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->AddTransrate(walkAnim->GetAnimIndex(), -1, 10);
    MV1PhysicsResetState(model);
}

int WalkState::ModelInitial()
{
    walkAnim = shared_ptr<PlayAnim>(new PlayAnim());
    auto ret = walkAnim->AttachAnime(model, (int)EAnimIndex::ANIM_WALK);
    walkAnim->SetPlaySpeed(1.0f);
    walkAnim->IsLoop(false);
    return ret;
}

void WalkState::SetDrawMMD(std::shared_ptr<DrawMMD> mmd)
{
    m_mmd = mmd;
}
