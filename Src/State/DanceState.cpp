#include "DanceState.h"

#include <iostream>
#include "DxLib.h"
#include <chrono>

#include "Util\UtilXml.h"
#include "Util\rapidxml-1.13\rapidxml_utils.hpp"

using namespace std;

chrono::time_point<chrono::system_clock> start;

void DanceState::Initialize()
{
    cout << "state: dance" << endl;
    isDance = true;

    auto anim = danceAnim[DanceIndex];
    anim->ResetAnimTime();
    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->AddTransrate(-1, anim->GetAnimIndex(), 1);
    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->AddAnim(anim);
    currentAnimIndex = anim->GetAnimIndex();

    start = chrono::system_clock::now();
    m_Output->Start(danceMusic[DanceIndex]);
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
        if (DanceIndex == 2)
        {
            isDance = true;
            animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->AddAnim(danceAnim[DanceIndex]);
            start = chrono::system_clock::now();
            if (!m_Output->IsPlay()) m_Output->Start(danceMusic[DanceIndex]);
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

#include <array>
int DanceState::ModelInitial()
{
    rapidxml::xml_document<> doc;
    rapidxml::file<> input("config.xml");
    doc.parse<0>(input.data());
    for (rapidxml::xml_node<>* child = doc.first_node("dance")->first_node();
        child != nullptr;
        child = child->next_sibling())
    {
        auto t = GetAttributes<int, std::wstring>(child, { "anim_num", "music_path" });

        auto animNum = std::get<0>(t);
        std::wstring musicPath(std::get<1>(t));

        auto danceAnimPtr = std::shared_ptr<PlayAnim>(new PlayAnim);
        danceAnimPtr->AttachAnime(model, animNum);
        danceAnimPtr->IsLoop(false);

        danceAnim.emplace_back(danceAnimPtr);
        danceMusic.emplace_back(musicPath);
    }
    return 0;
}

void DanceState::SetOutputSound(std::shared_ptr<OutputSound> output)
{
    m_Output = output;
}

void DanceState::SetDrawMMD(std::shared_ptr<DrawMMD> mmd)
{
    m_mmd = mmd;
}
