#include "ReadState.h"

#include <iostream>
#include "DxLib.h"

using namespace std;

void ReadState::Initialize()
{
    cout << "state: read" << endl;

    // ニュース原稿ダウンロード、ひらがな化
    system("python YahooAPI/yahooAPI.py");
    system("move text.txt data");
    system("move sep_text.txt data");

    // wav生成
    system("softalk_rec data/text.txt");

    // vmd生成
    system("TextLip.exe data/text.txt");
    system("move text.vmd data/text.vmd");
    system("motion_update.bat");

    // 各種読み込み
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
