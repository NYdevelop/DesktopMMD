#pragma once

#include "PlayAnimQueue.h"
#include <vector>

// 並列にアニメーションを実行するクラス
class ActionManager
{
public:

    enum EAnimQueue : int
    {
        QUEUE_BLINK = 0,
        QUEUE_BREATH,
        QUEUE_USE
    };

    ActionManager();
    ~ActionManager();

    void Play();

    void AddAnimQueue();
    std::shared_ptr<PlayAnimQueue> GetAnimQueue(EAnimQueue index);

private:
    std::vector<std::shared_ptr<PlayAnimQueue>> m_AnimVec;
};