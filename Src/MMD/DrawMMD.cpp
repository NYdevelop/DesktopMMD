#include "MMD/DrawMMD.h"

#include "Util/WinUtil.h"
#include <iostream>

#define M_PI       3.14159265358979323846f

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
}

void DrawMMD::afterInitialize()
{
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//�w�i��ݒ�(���߂�����)

    DxLib::SetDrawScreen(DX_SCREEN_BACK);//�`��Ώۂ𗠉�ʂɂ���

    RotateY = -2.7f;
    float posX = 0.f;
    float posY = -11.f;
    Zoom = 22.f;
    // Zoom = 1.7f;

    charaPos = VGet(posX, posY, 1.0f);

    SetupCamera_Ortho(Zoom);
    //SetupCamera_Perspective(Zoom);
    SetCameraNearFar(0.1f, 50.0f);
    SetCameraPositionAndTarget_UpVecY(cameraPos, VGet(0, 0, 0));
}

//���C���ƂȂ鏈��
int DrawMMD::mainProcess()
{
    if (isDraw == false) return S_OK;

    //�`����e��S�폜
    DxLib::ClearDrawScreen();
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//�w�i��ݒ�(���߂�����)

    SetupCamera_Ortho(Zoom);
    SetCameraPositionAndTarget_UpVecY(cameraPos, VGet(0, 0, 0));
    //SetupCamera_Perspective(Zoom);

    //���f���̍��W�w��
    DxLib::MV1SetPosition(model, charaPos);
    MV1SetRotationXYZ(model, VGet(0.0f, RotateY + M_PI, 0.0f));

    blink.PlayAnimation();

    m_StateManager->Doing();

    MV1PhysicsCalculation(model, 1000.0f / 60.0f);

    //���f���̕`��
    DxLib::MV1DrawModel(model);

    //�`����e����ʂɔ��f
    DxLib::ScreenFlip();

    return S_OK;
}

void DrawMMD::LoadModel()
{
    isDraw = false;

    MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
    MV1SetLoadModelPhysicsWorldGravity(-10.f);
    MV1InitModel();

    // TODO: �ݒ�t�@�C����
    MV1SetLoadModelAnimFilePath(L"../../C#/RealtimeListenMMD/Model/motion/motion");
    //model = MV1LoadModel(L"../../C#/RealtimeListenMMD/Model/�ۂ�Ւ�����a_����/�ۂ�Ւ�����a�Q����mode.pmx");
    //model = MV1LoadModel(L"../../C#/RealtimeListenMMD/Model/Menace ���i�X(�J�m������)/���i�X_edit.pmx");
    //model = MV1LoadModel(L"../../C#/RealtimeListenMMD/Model/�A�[���r�b�g��WF���ϗB�˕P(����ͱ)/�B�˕P(����ͱ)1.00.pmx");
    model = MV1LoadModel(L"../../C#/RealtimeListenMMD/Model/�^�k�L�ƃL�c�l ver.1/�L�c�l.pmx");

    if (model == -1)
    {
        MessageBox(NULL, TEXT("model load error"), NULL, MB_ICONERROR);
        return;
    }
    if (MV1GetAnimNum(model) == 0)
    {
        MessageBox(NULL, TEXT("motion load error"), NULL, MB_ICONERROR);
        return;
    }

    blink.AttachAnime(model, 2);
    blink.SetMaximumTime(250.f);

    MV1SetPosition(model, charaPos);
    MV1SetRotationXYZ(model, VGet(0.0f, RotateY + M_PI, 0.0f));

    MV1PhysicsResetState(model);

    isDraw = true;
}

void DrawMMD::UpdatePosRot()
{
    MV1SetPosition(model, charaPos);
    MV1SetRotationXYZ(model, VGet(0.0f, RotateY + M_PI, 0.0f));
}
