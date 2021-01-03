#include "DanceState.h"

#include <iostream>
#include "DxLib.h"

#include "Util\UtilXml.h"
#include "Util\rapidxml-1.13\rapidxml_utils.hpp"

using namespace std;


void DanceState::Initialize()
{
    cout << "state: dance" << endl;
    isDance = true;

    auto anim = std::get<DANCE_ANIM>(danceConfig[DanceIndex]);
    anim->ResetAnimTime();
    auto animQueue = animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE);
    animQueue->AddTransrate(-1, anim->GetAnimIndex(), 1);
    animQueue->AddAnim(anim);
    currentAnimIndex = anim->GetAnimIndex();

    start = chrono::system_clock::now();
    m_Output->Start(std::get<DANCE_MUSIC_PATH>(danceConfig[DanceIndex]));
}

void DanceState::Doing()
{
    if (isDance == false)
    {
        animManager->Play();
        return;
    }

    static const float MILLISEC_TO_FRAME = 1000.f / 30.f;
    auto tmp = animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE);
    if (tmp->Play(std::chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now() - start).count() / MILLISEC_TO_FRAME) == false)
    {
        isDance = false;
        if (std::get<DANCE_LOOP>(danceConfig[DanceIndex]) == true)
        {
            isDance = true;
            tmp->AddAnim(std::get<DANCE_ANIM>(danceConfig[DanceIndex]));
            start = chrono::system_clock::now();
            if (!m_Output->IsPlay()) m_Output->Start(std::get<DANCE_MUSIC_PATH>(danceConfig[DanceIndex]));
        }
    }
}

void DanceState::End()
{
    m_Output->Stop();
    m_Output->CloseDevice();

    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->SetCurrentStop();
    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->AddTransrate(currentAnimIndex, -1, 10);
    currentAnimIndex = -1;
}

int DanceState::ModelInitial()
{
    rapidxml::xml_document<> doc;
    rapidxml::file<> input("config.xml");
    doc.parse<0>(input.data());
    auto base = doc.first_node("DesktopMMD");
    NodeApply(base->first_node("dance")->first_node(), [&](auto child)
    {
        auto t = GetAttributes<int, std::wstring, bool>(child, { "anim_num", "music_path", "loop" }, {0, L"", false});

        auto animNum = std::get<DANCE_ANIM>(t);
        auto danceAnimPtr = std::shared_ptr<PlayAnim>(new PlayAnim);
        danceAnimPtr->AttachAnime(model, animNum);
        danceAnimPtr->IsLoop(false);
        danceConfig.emplace_back(std::tuple<std::shared_ptr < PlayAnim >, std::wstring, bool>(danceAnimPtr, std::get<DANCE_MUSIC_PATH>(t), std::get<DANCE_LOOP>(t)));
    });
    return 0;
}

void DanceState::SetOutputSound(std::shared_ptr<OutputSound> output)
{
    m_Output = output;
}

