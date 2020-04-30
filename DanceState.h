#pragma once

#include "StateMMD.h"

#include <memory>
#include "OutputSound.h"
#include "PlayAnim.h"

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