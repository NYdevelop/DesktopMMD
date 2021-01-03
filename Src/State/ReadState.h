#pragma once
#include "State/StateMMD.h"

#include <memory>
#include <chrono>
#include "Sound/OutputSound.h"
#include "MMD/Anim/PlayAnim.h"
#include "MMD/ManageMMD.h"

class ReadState : public StateMMD
{
public:
    void Initialize();
    void Doing();
    void End();

    int ModelInitial();

    void SetOutputSound(std::shared_ptr<OutputSound> output);

    inline void SetManager(ManageMMD* m)
    {
        manager = m;
    }

private:
    std::shared_ptr<OutputSound> m_Output;

    std::shared_ptr < PlayAnim >  lipAnim;

    ManageMMD* manager = nullptr;

    std::chrono::time_point<std::chrono::system_clock> start;
};