#include "WaitState.h"

#include <random>
#include "Util/UtilMMD.h"

static std::mt19937 mt;
static std::uniform_real_distribution<> randRange(0.967598021, 0.99);

void WaitState::Initialize()
{
    std::cout << "state: wait" << std::endl;

    MV1PhysicsResetState(model);

    /// カメラ方向を向く
    m_mmd->canViewCamera = true;
}

void WaitState::Doing()
{
    if (!animQueue->Empty()) return;
    m_mmd->canViewCamera = true;

    if (m_RandomMove == false)
    {
        if (mt() % 500 != 0) return;
        DoWaitAnim();
        return;
    }

    if (walkManager->IsMove() == true) return;

    if (mt() % 500 != 0) return;

    if (mt() % 2 == 0)
    {
        DoWaitAnim();
        return;
    }

    // ランダムなスクリーン座標へ移動開始
    auto z = (float)randRange(mt);
    WalkStart(
        VGet((float)(mt() % dispWidth), (float)(mt() % dispHeight), z),
        m_mmd.get(),
        walkManager);
}

void WaitState::End()
{
    m_mmd->canViewCamera = false;

    if (!animQueue->Empty())
    {
        animQueue->SetCurrentStop();
        MV1PhysicsResetState(model);
    }
}

void WaitState::OnceInitial()
{
    RECT rc;
    GetWindowRect(GetDesktopWindow(), &rc);
    dispWidth = rc.right - rc.left;
    dispHeight = rc.bottom - rc.top;

    std::random_device rnd;     // 非決定的な乱数生成器
    mt = move(std::mt19937(rnd()));            // メルセンヌ・ツイスタの32ビット版、引数は初期シード

    SetAnim(EAnimIndex::ANIM_THINK2);
    SetAnim(EAnimIndex::ANIM_LOOK_SELF1);
    SetAnim(EAnimIndex::ANIM_LOOK_SELF2);
    SetAnim(EAnimIndex::ANIM_DANCE_MINI);

    animQueue->SetDefAnimIndex(m_WaitAnimMap[0]->GetAnimIndex());
    MV1SetAttachAnimBlendRate(model, m_WaitAnimMap[0]->GetAnimIndex(), 1);
}

void WaitState::DoWaitAnim()
{
    m_mmd->canViewCamera = false;
    auto animIndex = mt() % m_WaitAnimMap.size(); //0;
    auto anim = m_WaitAnimMap[animIndex];
    std::cout << "wait anim : " << anim->GetAnimIndex() << std::endl;
    anim->ResetAnimTime();
    animQueue->AddTransrate(-1, anim->GetAnimIndex(), 10);
    animQueue->AddAnim(anim);
    if (animIndex == 3)
    {
        m_mmd->canViewCamera = true;
        int loopCount = mt() % 15 + 1;
        std::cout << "anim loop:" << loopCount << std::endl;
        for (int i = 0; i < loopCount; i++) animQueue->AddAnim(anim);
    }
    animQueue->AddTransrate(anim->GetAnimIndex(), -1, 10, true);
}

void WaitState::SetAnim(EAnimIndex index)
{
    int mapIndex = m_WaitAnimMap.size();
    m_WaitAnimMap[mapIndex] = std::shared_ptr<PlayAnim>(new PlayAnim);
    m_WaitAnimMap[mapIndex]->AttachAnime(model, (int)index);
    m_WaitAnimMap[mapIndex]->SetPlaySpeed(.5f);
    m_WaitAnimMap[mapIndex]->IsLoop(false);
}
