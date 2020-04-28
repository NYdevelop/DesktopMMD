#include "ReadState.h"

#include <iostream>
#include "DxLib.h"

using namespace std;

void ReadState::Initialize()
{
    cout << "state: read" << endl;

    // TODO: ニュース原稿ダウンロード
    // TODO: ひらがな化
    // TODO: wav, vmd生成
    // TODO: 読み込み
    manager->LoadModel();

    m_Output->Output(L"data/text.wav");
    MV1SetAttachAnimBlendRate(model, lipAnim.GetAnimIndex(), 1);
    lipAnim.ResetAnimTime();
}

void ReadState::Doing()
{
    // 口パクアニメ
    lipAnim.PlayAnimation();
}

void ReadState::End()
{
    m_Output->Stop();
    m_Output->CloseDevice();

    MV1SetAttachAnimBlendRate(model, lipAnim.GetAnimIndex(), 0);
}

void ReadState::ModelInitial()
{
    lipAnim.AttachAnime(model, 6);
    lipAnim.IsLoop(false);
    lipAnim.SetPlaySpeed(0.9f);
}



void ReadState::SetOutputSound(std::shared_ptr<OutputSound> output)
{
    m_Output = output;
}
