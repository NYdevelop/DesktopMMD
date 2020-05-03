#include "PlayAnimQueue.h"

#include "DxLib.h"
#include "MMD/PlayAnimTrans.h"

#include <iostream>

using namespace std;

void PlayAnimQueue::AddAnim(std::shared_ptr<AnimInterface> anim, bool isTransrate)
{
    m_Queue.push(std::pair<std::shared_ptr<AnimInterface>, bool>(anim, isTransrate));
}

void PlayAnimQueue::AddTransrate(int srcIndex, int transrateIndex, int time)
{
    if (m_Queue.empty() || m_Queue.front().second == false)
    {
        auto trans = std::shared_ptr<PlayAnimTrans>(new PlayAnimTrans);
        if (srcIndex != -1)
        {
            trans->srcAnimIndex = srcIndex;
        }
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

bool PlayAnimQueue::Play()
{
    if (m_Queue.empty()) return false;

    if (currentStop == true || m_Queue.front().first->PlayAnimation() == false)
    {
        currentStop = false;
        m_Queue.pop();
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
