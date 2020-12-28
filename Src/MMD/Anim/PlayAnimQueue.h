#pragma once

#include "Define.h"
#include "AnimInterface.h"
#include <queue>
#include <memory>

/// ��A�̓�����������邽�߂̃N���X
class PlayAnimQueue
{
public:
    void AddAnim(std::shared_ptr<AnimInterface> anim, bool isTransrate = false);

    void AddTransrate(int srcIndex, int transrateIndex, int time, bool notMarge = false);

    /// �L���[����ɂȂ�����false��Ԃ�
    bool Play();

    void Clear();

    void SetModel(int m);

    inline bool Empty() { return m_Queue.empty(); }

    inline void SetCurrentStop()
    {
        if (m_Queue.empty()) return;
        currentStop = true;
    }

    inline void SetDefAnimIndex(int index)
    {
        m_DefAnimIndex = index;
    }

private:
    /// �A�j���C���^�t�F�[�X��Transrate�t���O���i�[
    std::queue<std::pair <std::shared_ptr<AnimInterface>, bool>> m_Queue;
    int model = 0;
    bool currentStop = false;

    int m_DefAnimIndex = -1;
};