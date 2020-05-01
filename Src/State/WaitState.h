#pragma once

#include "StateMMD.h"

#include <iostream>
#include <memory>
#include "WalkStateManager.h"
#include "MMD/DrawMMD.h"

class WaitState : public StateMMD
{
public:
    inline void Initialize() { std::cout << "state: wait" << std:: endl; }
    void Doing();
    inline void End() {}
    inline void ModelInitial() {};

    void OnceInitial();

    inline void SetRandomMove(bool flag) { m_RandomMove = flag; };

    inline void SetDrawMMD(std::shared_ptr<DrawMMD> mmd)
    {
        m_mmd = mmd;
    }

    inline void SetWalkStateManager(WalkStateManager* m)
    {
        walkManager = m;
    }

private:
    bool m_RandomMove = false;
    std::shared_ptr<DrawMMD> m_mmd;
    WalkStateManager* walkManager;

    int dispWidth = 640;
    int dispHeight = 480;
};