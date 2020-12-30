#include "WaitState.h"

#include <random>
#include "Util/UtilMMD.h"

#include "Util\rapidxml-1.13\rapidxml.hpp"
#include "Util\rapidxml-1.13\rapidxml_utils.hpp"

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
    if (!animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->Empty()) return;
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

    if (!animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->Empty())
    {
        animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->SetCurrentStop();
        MV1PhysicsResetState(model);
    }
}

void WaitState::OnceInitial()
{
    RECT rc;
    GetWindowRect(GetDesktopWindow(), &rc);
    dispWidth = rc.right - rc.left;
    dispHeight = rc.bottom - rc.top;

    std::random_device rnd;          // 非決定的な乱数生成器
    mt = move(std::mt19937(rnd()));  // メルセンヌ・ツイスタの32ビット版、引数は初期シード

    // TODO: 設定値読込
    //rapidxml::xml_document<> doc;
    //rapidxml::file<> input("config_anim.xml");
    //doc.parse<0>(input.data());
    //for (rapidxml::xml_node<>* child = doc.first_node()->first_node();
    //    child != nullptr;
    //    child = child->next_sibling()) {
    //    for (auto attr = child->first_attribute(); attr != nullptr; attr = attr->next_attribute())
    //    {
    //        std::cout << attr->name() << ": " << attr->value() << std::endl;
    //    }
    //}
    SetAnim(EAnimIndex::ANIM_THINK2);
    SetAnim(EAnimIndex::ANIM_LOOK_SELF1);
    SetAnim(EAnimIndex::ANIM_LOOK_SELF2);
    SetAnim(EAnimIndex::ANIM_DANCE_MINI);
    SetAnim(EAnimIndex::ANIM_BONYARI);
    SetAnim(EAnimIndex::ANIM_LOOK_AROUND);
    SetAnim(EAnimIndex::ANIM_ARM_SWING);
    SetAnim(EAnimIndex::ANIM_STLETCH1);
    // SetAnim(EAnimIndex::ANIM_UMAUMA);
    // SetAnim(EAnimIndex::ANIM_PEACE1);
    // SetAnim(EAnimIndex::ANIM_SHIRATSUYU);
    // SetAnim(EAnimIndex::ANIM_PLINZ);
    // SetAnim(EAnimIndex::ANIM_KASHIMA);
    // SetAnim(EAnimIndex::ANIM_HAMAKAZE);
    // SetAnim(EAnimIndex::ANIM_ZERO_TWO_DANCE);

    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->SetDefAnimIndex(std::get<0>(m_WaitAnimMap[4])->GetAnimIndex());
    MV1SetAttachAnimBlendRate(model, std::get<0>(m_WaitAnimMap[4])->GetAnimIndex(), 1);
}

void WaitState::DoWaitAnim()
{
    auto animQueue = animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE);
    m_mmd->canViewCamera = false;
    auto animIndexMap = mt() % m_WaitAnimMap.size();
    auto anim = std::get<0>(m_WaitAnimMap[animIndexMap]);
    auto animIndex = std::get<1>(m_WaitAnimMap[animIndexMap]);
    std::cout << "wait anim : " << anim->GetAnimIndex() << std::endl;
    anim->ResetAnimTime();
    if (animIndex == EAnimIndex::ANIM_ZERO_TWO_DANCE)
    {
        animQueue->AddTransrate(-1, anim->GetAnimIndex(), 30);
    }
    else
    {
        animQueue->AddTransrate(-1, anim->GetAnimIndex(), 10);
    }
    animQueue->AddAnim(anim);
    if (animIndex == EAnimIndex::ANIM_DANCE_MINI)
    {
        m_mmd->canViewCamera = true;
        int loopCount = mt() % 15 + 1;
        std::cout << "anim loop:" << loopCount << std::endl;
        for (int i = 0; i < loopCount; i++) animQueue->AddAnim(anim);
    }
    if (animIndex == EAnimIndex::ANIM_ZERO_TWO_DANCE)
    {
        animQueue->AddTransrate(anim->GetAnimIndex(), -1, 30, true);
    }
    else
    {
        animQueue->AddTransrate(anim->GetAnimIndex(), -1, 10, true);
    }
}

void WaitState::SetAnim(EAnimIndex index)
{
    int mapIndex = m_WaitAnimMap.size();
    std::tuple<std::shared_ptr < PlayAnim >, EAnimIndex, bool, bool> tmp(std::shared_ptr<PlayAnim>(new PlayAnim), index, false, false);
    m_WaitAnimMap[mapIndex] = tmp;
    std::get<0>(m_WaitAnimMap[mapIndex])->AttachAnime(model, (int)index);
    std::get<0>(m_WaitAnimMap[mapIndex])->SetPlaySpeed(.5f);
    std::get<0>(m_WaitAnimMap[mapIndex])->IsLoop(false);
}
