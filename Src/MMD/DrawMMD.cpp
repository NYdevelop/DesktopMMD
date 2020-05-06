#include "MMD/DrawMMD.h"

#include "Util/WinUtil.h"
#include "Util/UtilMMD.h"
#include <iostream>

using namespace std;

//DxLib_Init�ȑO�ŌĂяo���ׂ�����
void DrawMMD::preInitialize()
{
    RECT rc;
    GetWindowRect(GetDesktopWindow(), &rc);
    dispWidth = rc.right - rc.left;
    dispHeight = rc.bottom - rc.top;
}

//int gHandle = 0;
void DrawMMD::afterInitialize()
{
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//�w�i��ݒ�(���߂�����)

    DxLib::SetDrawScreen(DX_SCREEN_BACK);//�`��Ώۂ𗠉�ʂɂ���

    RotateY = DX_PI_F;
    float posX = 0.f;
    float posY = 0.f;
    m_Zoom = 35.f;

    charaPos = VGet(posX, posY, 1.0f);
    cameraPos = VGet(0.f, 0.f, m_Zoom * -1.f);
    cameraViewOffset = VGet(0.f, 7.f, 1.f);
    cameraViewPos = VGet(0.f, 0.f, 0.f);

    SetupCamera_Perspective(30.f * DX_PI_F / 180.f);
    SetCameraNearFar(1.f, 1000.0f);
    SetCameraPositionAndTarget_UpVecY(cameraPos, cameraViewPos);

    //gHandle = MakeGraph(1920, 1040);
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

    SetCameraPositionAndTarget_UpVecY(
        cameraPos, VAdd(cameraViewPos, cameraViewOffset));

    //���f���̍��W�w��
    auto rayVec = cameraPos;
    //if (abs(rayVec.y+7.f) < 0.5)
    //{
    //    auto screenPos = ConvWorldPosToScreenPos(charaPos);
    //    screenPos.y = gravity.PosUpdate(screenPos.y);
    //}
    DxLib::MV1SetPosition(model, charaPos);
    MV1SetRotationXYZ(model, VGet(0.0f, RotateY + DX_PI_F, 0.0f));

    blink.PlayAnimation();

    if (canViewCamera)
    {
        ViewCamera(
            VSub(charaPos, GetRayVec()) ,
            MTranspose(MV1GetFrameLocalWorldMatrix(model, boneHead)),
            defHeadLocalRot, model, boneHead);
    }
    else
    {
        MV1ResetFrameUserLocalMatrix(model, boneHead);
    }

    m_AnimQueue->Play();
    m_StateManager->Doing();

    MV1PhysicsCalculation(model, 1000.0f / 60.0f);

    //���f���̕`��
    DxLib::MV1DrawModel(model);

    //GetDrawScreenGraph(0,0,1920,1040, gHandle);
    //GraphFilter(gHandle, DX_GRAPH_FILTER_GAUSS, 8, 50);
    //DrawGraph(0,0,gHandle, FALSE);

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

    boneHead = MV1SearchFrame(model, L"��");
    defHeadLocalRot = MV1GetFrameLocalMatrix(model, boneHead);

    blink.AttachAnime(model, (int)EAnimIndex::ANIM_BLINK);
    blink.SetMaximumTime(250.f);

    MV1SetPosition(model, charaPos);
    MV1SetRotationXYZ(model, VGet(0.0f, RotateY + DX_PI_F, 0.0f));

    MV1PhysicsResetState(model);

    isDraw = true;
}

void DrawMMD::UpdatePosRot()
{
    MV1SetPosition(model, charaPos);
    MV1SetRotationXYZ(model, VGet(0.0f, RotateY + DX_PI_F, 0.0f));
}
