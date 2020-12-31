#include "DanceState.h"

#include <iostream>
#include "DxLib.h"
#include <chrono>

#include "Util\rapidxml-1.13\rapidxml.hpp"
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
    for (rapidxml::xml_node<>* child = doc.first_node("dance")->first_node();
        child != nullptr;
        child = child->next_sibling())
    {
        int animNum = 0;
        std::wstring musicPath(L"");
        for (auto attr = child->first_attribute(); attr != nullptr; attr = attr->next_attribute())
        {
            std::string name(attr->name());
            if (name == "anim_num")
            {
                animNum = std::stoi(attr->value());
            }
            else if (name == "music_path")
            {
                std::string v(attr->value());
                const size_t len = v.size() + 1;
                TCHAR* tmp = new TCHAR[len];
                MultiByteToWideChar(CP_UTF8, 0, v.c_str(), -1, tmp, len);
                std::wstring ret(tmp);
                delete[] tmp;
                musicPath = ret;
            }
        }

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
