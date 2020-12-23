#include "MMD/DrawMMD.h"

#include "Util/WinUtil.h"
#include "Util/UtilMMD.h"
#include <iostream>

using namespace std;

DrawMMD::DrawMMD(const std::string & animPath, const std::string & modelPath)
{
    m_AnimPath = animPath;
    m_ModelPath = modelPath;

    //DxLib_Inità»ëOÇ≈åƒÇ—èoÇ∑Ç◊Ç´èàóù
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
        GetColor(1, 1, 1), TRUE);//îwåiÇê›íË(ìßâﬂÇ≥ÇπÇÈ)

    DxLib::SetDrawScreen(DX_SCREEN_BACK);//ï`âÊëŒè€Çó†âÊñ Ç…Ç∑ÇÈ

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

//ÉÅÉCÉìÇ∆Ç»ÇÈèàóù
int DrawMMD::mainProcess()
{
    if (isDraw == false) return S_OK;

    //ï`âÊì‡óeÇëSçÌèú
    DxLib::ClearDrawScreen();
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//îwåiÇê›íË(ìßâﬂÇ≥ÇπÇÈ)

    SetCameraPositionAndTarget_UpVecY(
        cameraPos, VAdd(cameraViewPos, cameraViewOffset));

    //ÉÇÉfÉãÇÃç¿ïWéwíË
    auto rayVec = GetRayVec();
    if (abs(atan2(rayVec.z, rayVec.y) + DX_PI_F / 2) < DX_PI_F / 4)
    {
        charaPos.y = gravity.PosUpdate(charaPos);
    }
    DxLib::MV1SetPosition(model, charaPos);
    MV1SetRotationXYZ(model, VGet(0.0f, RotateY + DX_PI_F, 0.0f));

    blink.PlayAnimation();

    if (canViewCamera)
    {
        ViewCamera(
            VSub(charaPos, rayVec) ,
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

    //ÉÇÉfÉãÇÃï`âÊ
    DxLib::MV1DrawModel(model);

    //GetDrawScreenGraph(0,0,1920,1040, gHandle);
    //GraphFilter(gHandle, DX_GRAPH_FILTER_GAUSS, 8, 50);
    //DrawGraph(0,0,gHandle, FALSE);

    //ï`âÊì‡óeÇâÊñ Ç…îΩâf
    DxLib::ScreenFlip();

    return S_OK;
}

void DrawMMD::LoadModel()
{
    isDraw = false;

    MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
    //MV1SetLoadModelPhysicsWorldGravity(-10.f);
    MV1InitModel();

    TCHAR buf[255];
    MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, m_AnimPath.c_str(), -1, buf, 255);
    MV1SetLoadModelAnimFilePath(buf);

    MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, m_ModelPath.c_str(), -1, buf, 255);
    model = MV1LoadModel(buf);

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

    boneHead = MV1SearchFrame(model, L"ì™");
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
