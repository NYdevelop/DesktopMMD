#pragma once

#include "State/StateMMD.h"

#include <memory>
#include "Sound/OutputSound.h"
#include "MMD/PlayAnim.h"

class DanceState : public StateMMD
{
public:
    void Initialize();
    void Doing();
    void End();

    void ModelInitial();

    void SetOutputSound(std::shared_ptr<OutputSound> output);

private:
    std::shared_ptr<OutputSound> m_Output;

    PlayAnim danceAnim;
};