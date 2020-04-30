#include "DanceState.h"

#include <iostream>
#include "DxLib.h"
#include <chrono>

using namespace std;

void DanceState::Initialize()
{
    cout << "state: dance" << endl;

    // TODO: モデル位置リセット
    MV1SetAttachAnimBlendRate(model, danceAnim.GetAnimIndex(), 1);
    danceAnim.ResetAnimTime();
    MV1PhysicsResetState(model);

    m_Output->Output(L"data/music.wav");
}


void DanceState::Doing()
{
    danceAnim.PlayAnimation();
}

void DanceState::End()
{
    m_Output->Stop();
    m_Output->CloseDevice();

    MV1SetAttachAnimBlendRate(model, danceAnim.GetAnimIndex(), 0);
    MV1PhysicsResetState(model);
}

void DanceState::ModelInitial()
{
    danceAnim.AttachAnime(model, 7);
    danceAnim.IsLoop(false);
    danceAnim.SetPlaySpeed(.575f);
    MV1SetAttachAnimBlendRate(model, danceAnim.GetAnimIndex(), 0);
}

void DanceState::SetOutputSound(std::shared_ptr<OutputSound> output)
{
    m_Output = output;
}
