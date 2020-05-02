#pragma once

#include "PlayAnim.h"
#include <queue>
#include <memory>

/// 一連の動作を実現するためのクラス
class PlayAnimQueue
{
public:
    void AddAnim(std::shared_ptr<PlayAnim> anim);

    /// キューが空になったらfalseを返す
    bool Play();

    void Clear();

    void SetModel(int m);

private:
    std::queue<std::shared_ptr<PlayAnim>> m_Queue;
    int model = 0;
};