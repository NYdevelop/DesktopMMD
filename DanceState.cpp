#include "DanceState.h"

#include <iostream>
#include "DxLib.h"

using namespace std;

void DanceState::Initialize()
{
    cout << "state: dance" << endl;

    m_Output->Output(L"data/music.wav");
    MV1SetAttachAnimBlendRate(model, danceAnim.GetAnimIndex(), 1);
    danceAnim.ResetAnimTime();
    MV1PhysicsResetState(model);
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
    danceAnim.SetPlaySpeed(.5f);
    MV1SetAttachAnimBlendRate(model, danceAnim.GetAnimIndex(), 0);
}

void DanceState::SetOutputSound(std::shared_ptr<OutputSound> output)
{
    m_Output = output;
}
