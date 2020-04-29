#include "ReadState.h"

#include <iostream>
#include "DxLib.h"

using namespace std;

void ReadState::Initialize()
{
    cout << "state: read" << endl;

    // �j���[�X���e�_�E�����[�h�A�Ђ炪�ȉ�
    system("python YahooAPI/yahooAPI.py");
    system("move text.txt data");
    system("move sep_text.txt data");

    // wav����
    system("softalk_rec data/text.txt");

    // vmd����
    system("TextLip.exe data/text.txt");
    system("move text.vmd data/text.vmd");
    system("motion_update.bat");

    // �e��ǂݍ���
    manager->LoadModel();
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
    lipAnim.SetPlaySpeed(0.9f);
}



void ReadState::SetOutputSound(std::shared_ptr<OutputSound> output)
{
    m_Output = output;
}
