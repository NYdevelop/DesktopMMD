#include "WalkState.h"

#include <iostream>
#include "DxLib.h"

#include "Util/WinUtil.h"
#include "MMD/PlayAnimTrans.h"
#include "Define.h"

#define M_PI       3.14159265358979323846f

using namespace std;

void WalkState::Initialize()
{
    cout << "state: walk" << endl;

    m_mmd->RotateY = m_Direction;
    m_mmd->UpdatePosRot();
    MV1PhysicsResetState(model);

    walkAnim->ResetAnimTime();

    // TODO: 前状態の終了処理をここで行うのはおかしい Endにて実行または情報を登録しておくべきか
    auto trans = shared_ptr<PlayAnimTrans>(new PlayAnimTrans);
    auto animVec = (*animationMap)[stateManager->GetPreviousStateIndex()];
    if (animVec.size() != 0)
    {
        trans->SetSrcAnimIndex((int)animVec[0]);
    }
    trans->AttachAnime(model, walkAnim->GetAnimIndex());
    trans->SetTransTime(10);
    animQueue->AddAnim(trans);

    animQueue->AddAnim(walkAnim);
}

void WalkState::Doing()
{
    if (animQueue->Empty())
    {
        walkAnim->ResetAnimTime();
        animQueue->AddAnim(walkAnim);
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
    // TODO: 自然な待機モーションへの移行
    //auto trans = shared_ptr<PlayAnimTrans>(new PlayAnimTrans);
    //trans->SetSrcAnimIndex(walkAnim->GetAnimIndex());
    //trans->AttachAnime(model, 0);
    //trans->SetTransTime(10);
    //animQueue->AddAnim(trans);
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
