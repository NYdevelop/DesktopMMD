#pragma once

#include "Define.h"
#include "AnimInterface.h"
#include <queue>
#include <memory>

/// ��A�̓�����������邽�߂̃N���X
class PlayAnimQueue
{
public:
    void AddAnim(std::shared_ptr<AnimInterface> anim);

    /// �L���[����ɂȂ�����false��Ԃ�
    bool Play();

    void Clear();

    void SetModel(int m);

    bool Empty() { return m_Queue.empty(); }

private:
    std::queue<std::shared_ptr<AnimInterface>> m_Queue;
    int model = 0;
};