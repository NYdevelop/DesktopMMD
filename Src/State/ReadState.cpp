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
        // �j���[�X���e�_�E�����[�h�A�Ђ炪�ȉ�
        system("python YahooAPI/yahooAPI.py");
        system("move text.txt data");
        system("move sep_text.txt data");
    }

    if (IsSame("data/text.txt", "data/before_text.txt") == false)
    {
        thread t([this]()
            {
                // vmd����
                system("TextLip.exe data/sep_text.txt");
                system("move text.vmd data/text.vmd");
                system("motion_update.bat");

                // �e��ǂݍ���
                manager->LoadModel();
            });

        // wav����
        system("softalk_rec data\\text.txt");

        system("copy /Y data\\text.txt data\\before_text.txt");
        t.join();
    }

    m_Output->Output(L"data/text.wav");
    MV1SetAttachAnimBlendRate(model, lipAnim.GetAnimIndex(), 1);
    lipAnim.ResetAnimTime();
}

void ReadState::Doing()
{
    // ���p�N�A�j��
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
    lipAnim.SetPlaySpeed(1.0f);
}



void ReadState::SetOutputSound(std::shared_ptr<OutputSound> output)
{
    m_Output = output;
}
