#pragma once

#include "PlayAnimQueue.h"
#include <vector>

// 並列にアニメーションを実行するためのクラス
class ActionManager
{
    void Play();

private:
    std::vector<PlayAnimQueue> m_AnimVec;
};