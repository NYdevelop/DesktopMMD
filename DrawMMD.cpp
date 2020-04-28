#include "DrawMMD.h"

#include "WinUtil.h"
//#include <string>
#include <iostream>

using namespace std;

//DxLib_Init�ȑO�ŌĂяo���ׂ�����
void DrawMMD::preInitialize()
{
    //�f�B�X�v���C�𑜓x�̎擾
    // http://detail.chiebukuro.yahoo.co.jp/qa/question_detail/q10129265945
    RECT rc;
    GetWindowRect(GetDesktopWindow(), &rc);
    dispWidth = rc.right - rc.left;
    dispHeight = rc.bottom - rc.top;

    //�𑜓x�C�E�B���h�E���[�h�w��
    //DxLib::ChangeWindowMode(TRUE); // �E�B���h�E���[�h�ɕύX
    //DxLib::SetGraphMode(dispWidth, dispHeight, 32); // �𑜓x�̐ݒ�

    //http://www.sys-cube.co.jp/blog/5347.html
    //DxLib::SetWindowStyleMode(2); // �E�B���h�E�̃X�^�C����g�����Ɏw��

    // �d��������
    //DxLib::SetUseBackBufferTransColorFlag(TRUE); // �E�B���h�E�𓧖��Ɏw��
}

void DrawMMD::afterInitialize()
{
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//�w�i��ݒ�(���߂�����)

    //3D����̏�����
    //DxLib::SetUseZBuffer3D(TRUE);
    //DxLib::SetWriteZBuffer3D(TRUE);

    //�����T�C�Y�̐ݒ�
    //SetFontSize(32);

    //�E�B���h�E���őO�ʂɕ\������
    // http://dixq.net/forum/viewtopic.php?f=3&t=10766
    HWND hWnd = GetMainWindowHandle();
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    //DxLib::SetAlwaysRunFlag(TRUE);//�E�B���h�E���A�N�e�B�u��ԂłȂ��Ă����s����
    DxLib::SetDrawScreen(DX_SCREEN_BACK);//�`��Ώۂ𗠉�ʂɂ���

    MV1SetLoadModelAnimFilePath(L"../C#/RealtimeListenMMD/Model/motion/motion");
    model = MV1LoadModel(L"../C#/RealtimeListenMMD/Model/�����䂩��_��_ver1.0/�����䂩��_��.pmd");//���f���f�[�^�̓ǂݍ���
                                                                                             //�A�j���[�V�����ݒ�
                                                                                             // http://dixq.net/g/3d_01.html

    if (model == -1)
    {
        throw "model load error";
    }
    if (MV1GetAnimNum(model) == 0)
    {
        throw "motion load error";
    }

    attachIndex = DxLib::MV1AttachAnim(model, 3, -1, FALSE);
    totalTime = DxLib::MV1GetAttachAnimTotalTime(model, attachIndex);
    playTime = 0.0f;
    playSpeed = BASE_PLAY_SPEED;
    cout << BASE_PLAY_SPEED << endl;
    m_Bpm = BASE_BPM;

    //�֊s���̑傫�����C������
    //�ʏ�MV1SetScale�Ŋg�傷��ƁC�֊s���̑傫�����g�傳��Ă��܂�
    //�S�Ă̗֊s���ɃA�N�Z�X���CMV1SetScale�Ŋg�債���l�Ŋ��邱�ƂŁC���������Ƃɖ߂�
    // http://peroon.hatenablog.com/entry/20110402/1301773655
    //int MaterialNum = DxLib::MV1GetMaterialNum(model);
    //for (int i = 0; i < MaterialNum; i++)
    //{
    //    // �}�e���A���̗֊s���̑������擾 
    //    float dotwidth = DxLib::MV1GetMaterialOutLineDotWidth(model, i);
    //    // �}�e���A���̗֊s���̑������g�債�������������� 
    //    DxLib::MV1SetMaterialOutLineDotWidth(model, i, dotwidth / scale);
    //}

    float rotateY = -5.8f;
    float posX = 3.7f;////6.5f;//15f;
    float posY = -18.7f;//-20.3f;//-25f;
    float zoom = 6.8f;//10f;//20f;
    charaPos = VGet(posX, posY, 1.0f);
    SetupCamera_Ortho(6.8f);
    MV1SetPosition(model, charaPos);
    MV1SetRotationXYZ(model, VGet(0.0f, rotateY, 0.0f));
    SetCameraNearFar(0.1f, 50.0f);
    SetCameraPositionAndTarget_UpVecY(VGet(0, 0, -10), VGet(0, 0, 0));
}

//���C���ƂȂ鏈��
int DrawMMD::mainProcess()
{
    //�`����e��S�폜
    DxLib::ClearDrawScreen();
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//�w�i��ݒ�(���߂�����)

    //���f���̈ړ�
    //const float speed = 0.1f;
    //if (DxLib::CheckHitKey(KEY_INPUT_RIGHT))charaPos.x += speed;
    //if (DxLib::CheckHitKey(KEY_INPUT_LEFT))charaPos.x -= speed;

    //���f���̍��W�w��
    DxLib::MV1SetPosition(model, charaPos);

    //���f���̃A�j���[�V�����ݒ�
    //http://dixq.net/g/3d_01.html
    // �Đ����Ԃ�i�߂�
    // ����1.0�͂��������ɓ����悤�ɓK���Ɍ��߂�
    // ���ɂ���ď��������ȂǂŎ��s���x���ς�邱�Ƃ�����
    playTime += playSpeed;
    // �Đ����Ԃ��A�j���[�V�����̑��Đ����ԂɒB������Đ����Ԃ��O�ɖ߂�
    if (playTime >= totalTime) {
        playTime = 0.0f;
    }
    // �Đ����Ԃ��Z�b�g����
    DxLib::MV1SetAttachAnimTime(model, attachIndex, playTime);

    //���f���̕`��
    DxLib::MV1DrawModel(model);

    //DrawString(
    //    1750, 1000,
    //    StringToWString(std::to_string(m_Bpm)).c_str(),
    //    GetColor(0, 0, 0));

    //�N���b�N�ɔ���
    //if (DxLib::GetMouseInput()&MOUSE_INPUT_LEFT)
    //{
    //    //�L�����N�^�[�́u�X�N���[����v�ł̍��W���擾����
    //    DxLib::VECTOR pos = DxLib::ConvWorldPosToScreenPos(charaPos);
    //    DxLib::DrawFormatString(pos.x, pos.y-600, 0x000000, L">>�N���b�N����Ă��`");
    //}

    //�`����e����ʂɔ��f
    DxLib::ScreenFlip();

    //if (DxLib::CheckHitKey(KEY_INPUT_Q)) return -1;
    return S_OK;
}

void DrawMMD::SetBPM(float bpm)
{
    if (bpm > 140) bpm /= 2.0f;
    playSpeed = BASE_PLAY_SPEED * bpm / BASE_BPM;
    m_Bpm = bpm;
}
