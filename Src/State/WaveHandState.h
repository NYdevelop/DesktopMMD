#pragma once

#include "StateMMD.h"

#include "MMD/DrawMMD.h"
#include "MMD/PlayAnimQueue.h"

class WaveHandState : public StateMMD
{
public:
    void Initialize();
    void Doing();
    void End();

    void ModelInitial();

    void SetDrawMMD(std::shared_ptr<DrawMMD> mmd);

private:
    std::shared_ptr<DrawMMD> m_mmd;

    PlayAnimQueue anim;
    std::shared_ptr<PlayAnim> waveHandLoop;

    int progressTime = 0;
    int transrateTime = 20;
};