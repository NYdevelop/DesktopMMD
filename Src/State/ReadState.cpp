#include "State/ReadState.h"

#include <iostream>
#include "DxLib.h"

using namespace std;

bool IsSame(string beforeFileName, string fileName)
{
    ifstream ifsBefore(beforeFileName);
    std::string beforeText((std::istreambuf_iterator<char>(ifsBefore)),
        std::istreambuf_iterator<char>());

    ifstream ifs(fileName);
    std::string text((std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>());

    return beforeText == text;
}

void ReadState::Initialize()
{
    cout << "state: read" << endl;

    if (IsSame("data/text.txt", "data/before_text.txt") == true)
    {
        // ニュース原稿ダウンロード、ひらがな化
        system("python YahooAPI/yahooAPI.py");
        system("move text.txt data");
        system("move sep_text.txt data");
    }

    if (IsSame("data/text.txt", "data/before_text.txt") == false)
    {
        thread t([this]()
            {
                // vmd生成
                system("TextLip.exe data/sep_text.txt");
                system("move text.vmd data/text.vmd");
                system("motion_update.bat");

                // 各種読み込み
                manager->LoadModel();
            });

        // wav生成
        system("softalk_rec data\\text.txt");

        system("copy /Y data\\text.txt data\\before_text.txt");
        t.join();
    }

    m_Output->Start(L"data/text.wav");
    MV1SetAttachAnimBlendRate(model, lipAnim->GetAnimIndex(), 1);
    lipAnim->ResetAnimTime();
    animQueue->AddAnim(lipAnim);
}

void ReadState::Doing()
{
}

void ReadState::End()
{
    m_Output->Stop();
    m_Output->CloseDevice();

    animQueue->SetCurrentStop();
    animQueue->AddTransrate(lipAnim->GetAnimIndex(), -1, 10);
}

int ReadState::ModelInitial()
{
    lipAnim = std::shared_ptr<PlayAnim>(new PlayAnim);
    int ret = lipAnim->AttachAnime(model, (int)EAnimIndex::ANIM_LIP);
    lipAnim->IsLoop(false);
    lipAnim->SetPlaySpeed(1.0f);
    return ret;
}

void ReadState::SetOutputSound(std::shared_ptr<OutputSound> output)
{
    m_Output = output;
}
