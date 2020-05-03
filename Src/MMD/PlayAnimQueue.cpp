#include "PlayAnimQueue.h"

#include "DxLib.h"

void PlayAnimQueue::AddAnim(std::shared_ptr<AnimInterface> anim)
{
    m_Queue.push(anim);
}

bool PlayAnimQueue::Play()
{
    if (m_Queue.empty()) return false;

    if (m_Queue.front()->PlayAnimation() == false)
    {
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
            model, m_Queue.front()->GetAnimIndex(), 0);
        m_Queue.pop();
    }
}

void PlayAnimQueue::SetModel(int m)
{
    model = m;
}
