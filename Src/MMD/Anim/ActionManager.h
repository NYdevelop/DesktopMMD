#pragma once

#include "PlayAnimQueue.h"
#include <vector>

// ����ɃA�j���[�V���������s���邽�߂̃N���X
class ActionManager
{
    void Play();

private:
    std::vector<PlayAnimQueue> m_AnimVec;
};