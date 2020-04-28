#pragma once
#include "StateMMD.h"

#include <memory>
#include "OutputSound.h"
#include "PlayAnim.h"

#include "ManageMMD.h"

class ReadState : public StateMMD
{
public:
    void Initialize();
    void Doing();
    void End();

    void ModelInitial();

    void SetOutputSound(std::shared_ptr<OutputSound> output);

    inline void SetManager(ManageMMD* m)
    {
        manager = m;
    }

private:
    std::shared_ptr<OutputSound> m_Output;

    PlayAnim lipAnim;

    ManageMMD* manager;
};