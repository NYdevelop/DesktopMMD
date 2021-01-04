#include "PlayAnimQueue.h"

#include "DxLib.h"
#include "MMD/Anim/PlayAnimTrans.h"

#include <iostream>

using namespace std;

void PlayAnimQueue::AddAnim(std::shared_ptr<AnimInterface> anim, bool isTransrate)
{
    m_Queue.push(std::pair<std::shared_ptr<AnimInterface>, bool>(anim, isTransrate));
}

void PlayAnimQueue::AddTransrate(int srcIndex, int transrateIndex, int time, bool notMarge)
{
    if (m_Queue.empty() || m_Queue.front().second == false || notMarge == true)
    {
        auto trans = std::shared_ptr<PlayAnimTrans>(new PlayAnimTrans);
        trans->srcAnimIndex = m_DefAnimIndex;
        if (srcIndex != -1)
        {
            trans->srcAnimIndex = srcIndex;
        }
        if (transrateIndex == -1)
        {
            transrateIndex = m_DefAnimIndex;
        }
        if (trans->srcAnimIndex == transrateIndex) return;
        trans->AttachAnime(model, transrateIndex);
        trans->SetTransTime(time);
        AddAnim(trans, true);
        return;
    }

    // Šù‚ÉˆÚs‚µ‚Ä‚¢‚éê‡
    auto trans = (PlayAnimTrans*)m_Queue.front().first.get();
    if (trans->transrateAnimIndex != -1)
    {
        cerr << "already set transrateIndex: " << trans->transrateAnimIndex << endl;
    }
    trans->transrateAnimIndex = transrateIndex;
}

bool PlayAnimQueue::Play(float time)
{
    if (m_Queue.empty()) return false;

    if (currentStop == true || m_Queue.front().first->PlayAnimation(time) == false)
    {
        if (currentStop)
        {
            while (m_Queue.front().second == false)
            {
                m_Queue.pop();
            }
        }
        else
        {
            m_Queue.pop();
        }

        currentStop = false;
        if (m_Queue.empty()) return false;
    }
    return true;
}

void PlayAnimQueue::Clear()
{
    while (!m_Queue.empty())
    {
        MV1SetAttachAnimBlendRate(
            model, m_Queue.front().first->GetAnimIndex(), 0);
        m_Queue.pop();
    }
}

void PlayAnimQueue::SetModel(int m)
{
    model = m;
}

void PlayAnimQueue::Pause()
{
    auto index = m_Queue.front().first->GetAnimIndex();
    MV1SetAttachAnimBlendRate(model, index, 0);
    isPause = true;
}

void PlayAnimQueue::Resume()
{
    if (isPause == false) return;
    auto index = m_Queue.front().first->GetAnimIndex();
    MV1SetAttachAnimBlendRate(model, index, 1);
    isPause = false;
}