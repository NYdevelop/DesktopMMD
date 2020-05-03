#pragma once

#include "Define.h"
#include "AnimInterface.h"
#include <queue>
#include <memory>

/// 一連の動作を実現するためのクラス
class PlayAnimQueue
{
public:
    void AddAnim(std::shared_ptr<AnimInterface> anim);

    /// キューが空になったらfalseを返す
    bool Play();

    void Clear();

    void SetModel(int m);

    bool Empty() { return m_Queue.empty(); }

private:
    std::queue<std::shared_ptr<AnimInterface>> m_Queue;
    int model = 0;
};