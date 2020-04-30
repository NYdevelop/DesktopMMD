#pragma once

#include "State/StateMMD.h"

#include <memory>
#include "Sound/OutputSound.h"
#include "MMD/PlayAnim.h"
#include "MMD/DrawMMD.h"

class DanceState : public StateMMD
{
public:
    void Initialize();
    void Doing();
    void End();

    void ModelInitial();

    void SetOutputSound(std::shared_ptr<OutputSound> output);
    void SetDrawMMD(std::shared_ptr<DrawMMD> mmd);

private:
    std::shared_ptr<OutputSound> m_Output;
    std::shared_ptr<DrawMMD> m_mmd;

    PlayAnim danceAnim;
};