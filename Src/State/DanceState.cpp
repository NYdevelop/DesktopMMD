#include "DanceState.h"

#include <iostream>
#include "DxLib.h"
#include <chrono>

using namespace std;

chrono::time_point<chrono::system_clock> start;

void DanceState::Initialize()
{
    cout << "state: dance" << endl;
    isDance = true;

    danceAnim->ResetAnimTime();
    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->AddTransrate(-1, danceAnim->GetAnimIndex(), 1);
    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->AddAnim(danceAnim);

    start = chrono::system_clock::now();
    m_Output->Start(L"data/music.wav");
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
    animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_USE)->AddTransrate(danceAnim->GetAnimIndex(), -1, 10);
}

int DanceState::ModelInitial()
{
    // TODO: ƒ_ƒ“ƒX‚ð‘I‘ð‚Å‚«‚é‚æ‚¤‚É
    danceAnim = std::shared_ptr<PlayAnim>(new PlayAnim);
    int ret = danceAnim->AttachAnime(model, (int)EAnimIndex::ANIM_DANCE2);
    danceAnim->IsLoop(false);
    danceAnim->SetPlaySpeed(.575f);
    return ret;
}

void DanceState::SetOutputSound(std::shared_ptr<OutputSound> output)
{
    m_Output = output;
}

void DanceState::SetDrawMMD(std::shared_ptr<DrawMMD> mmd)
{
    m_mmd = mmd;
}
