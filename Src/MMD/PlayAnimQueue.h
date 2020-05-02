#pragma once

#include "PlayAnim.h"
#include <queue>
#include <memory>

/// ��A�̓�����������邽�߂̃N���X
class PlayAnimQueue
{
public:
    void AddAnim(std::shared_ptr<PlayAnim> anim);

    /// �L���[����ɂȂ�����false��Ԃ�
    bool Play();

    void Clear();

    void SetModel(int m);

private:
    std::queue<std::shared_ptr<PlayAnim>> m_Queue;
    int model = 0;
};