#include "ActionManager.h"

ActionManager::ActionManager()
{
    AddAnimQueue();
    AddAnimQueue();
    AddAnimQueue();
}

ActionManager::~ActionManager()
{
}

void ActionManager::Play()
{
    for (auto& q : m_AnimVec)
    {
        q->Play();
    }
}

void ActionManager::AddAnimQueue()
{
    m_AnimVec.emplace_back(std::shared_ptr<PlayAnimQueue>(new PlayAnimQueue));
}

std::shared_ptr<PlayAnimQueue> ActionManager::GetAnimQueue(EAnimQueue index)
{
    return m_AnimVec[index];
}
