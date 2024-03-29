#pragma once

#include "State/StateMMD.h"

#include <memory>
#include <chrono>
#include "Sound/OutputSound.h"
#include "MMD/Anim/PlayAnim.h"
#include "MMD/DrawMMD.h"

class DanceState : public StateMMD
{
public:
    void Initialize();
    void Doing();
    void End();

    int ModelInitial();

    void SetOutputSound(std::shared_ptr<OutputSound> output);

    int DanceIndex = 0;

private:
    std::shared_ptr<OutputSound> m_Output;

    enum EDanceConfig : int
    {
        DANCE_ANIM = 0,
        DANCE_MUSIC_PATH,
        DANCE_LOOP,
    };
    std::vector<std::tuple<std::shared_ptr < PlayAnim >, std::wstring, bool > > danceConfig;

    int currentAnimIndex = -1;
    bool isDance = false;
    std::chrono::time_point<std::chrono::system_clock> start;
};