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
    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_BLINK)->Restart();
    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_BREATH)->Restart();

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

    // 設定値読込
    loadConfig();

    auto animIndex = std::get<EAnimMap::ITEM_ANIM>(m_WaitAnimMap[4])->GetAnimIndex();
    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->SetDefAnimIndex(animIndex);
    MV1SetAttachAnimBlendRate(model, animIndex, 1);
}

void WaitState::DoWaitAnim()
{
    auto animQueue = animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE);
    m_mmd->canViewCamera = false;
    auto animIndexMap = mt() % m_WaitAnimMap.size();
    auto tmpTuple = m_WaitAnimMap[animIndexMap];
    auto anim = std::get<EAnimMap::ITEM_ANIM>(tmpTuple);
    anim->ResetAnimTime();
    auto animIndex = std::get<EAnimMap::ITEM_INDEX>(tmpTuple);
    std::cout << "wait anim : " << (int)animIndex << std::endl;

    auto transframe = std::get<EAnimMap::ITEM_TRANSE_FRAME>(tmpTuple);
    animQueue->AddTransrate(-1, anim->GetAnimIndex(), transframe);
    animQueue->AddAnim(anim);
    if (animIndex == EAnimIndex::ANIM_DANCE_MINI)
    {
        m_mmd->canViewCamera = true;
        int loopCount = mt() % 15 + 1;
        std::cout << "anim loop:" << loopCount << std::endl;
        for (int i = 0; i < loopCount; i++) animQueue->AddAnim(anim);
    }
    animQueue->AddTransrate(anim->GetAnimIndex(), -1, transframe, true);

    if (std::get<EAnimMap::ITEM_BLINK>(tmpTuple) == false)
    {
        animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_BLINK)->Pause();
    }
    if (std::get<EAnimMap::ITEM_BREATH>(tmpTuple) == false)
    {
        animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_BREATH)->Pause();
    }
}

void WaitState::SetAnim(EAnimIndex index, bool isBlink, bool isBreath, int transFrame)
{
    int mapIndex = m_WaitAnimMap.size();
    std::tuple<std::shared_ptr < PlayAnim >, EAnimIndex, bool, bool, int> tmp(std::shared_ptr<PlayAnim>(new PlayAnim), index, isBlink, isBreath, transFrame);
    m_WaitAnimMap[mapIndex] = tmp;
    std::get<EAnimMap::ITEM_ANIM>(m_WaitAnimMap[mapIndex])->AttachAnime(model, (int)index);
    std::get<EAnimMap::ITEM_ANIM>(m_WaitAnimMap[mapIndex])->SetPlaySpeed(.5f);
    std::get<EAnimMap::ITEM_ANIM>(m_WaitAnimMap[mapIndex])->IsLoop(false);
}

void WaitState::loadConfig()
{
    rapidxml::xml_document<> doc;
    rapidxml::file<> input("config_anim.xml");
    doc.parse<0>(input.data());
    for (rapidxml::xml_node<>* child = doc.first_node()->first_node();
        child != nullptr;
        child = child->next_sibling())
    {
        std::string name("");
        int animNum = 0;
        bool isBlink = true;
        bool isBreath = true;
        int transframe = 10;

        for (auto attr = child->first_attribute(); attr != nullptr; attr = attr->next_attribute())
        {
            //std::cout << attr->name() << ": " << attr->value() << std::endl;
            std::string name(attr->name());
            if (name == "num")
            {
                animNum = std::stoi(attr->value());
            }
            else if (name == "blink")
            {
                isBlink = attr->value() == "false" ? false : true;
            }
            else if (name == "breath")
            {
                isBreath = attr->value() == "false" ? false : true;
            }
            else if (name == "transframe")
            {
                transframe = std::stoi(attr->value());
            }
        }
        SetAnim((EAnimIndex)animNum, isBlink, isBreath, transframe);
    }
}
