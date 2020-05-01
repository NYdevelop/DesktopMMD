#include "MMD/DrawMMD.h"

#include "Util/WinUtil.h"
#include <iostream>

#define M_PI       3.14159265358979323846f

using namespace std;

//DxLib_Init以前で呼び出すべき処理
void DrawMMD::preInitialize()
{
    //ディスプレイ解像度の取得
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
        GetColor(1, 1, 1), TRUE);//背景を設定(透過させる)

    DxLib::SetDrawScreen(DX_SCREEN_BACK);//描画対象を裏画面にする

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

//メインとなる処理
int DrawMMD::mainProcess()
{
    if (isDraw == false) return S_OK;

    //描画内容を全削除
    DxLib::ClearDrawScreen();
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//背景を設定(透過させる)

    SetupCamera_Ortho(Zoom);
    SetCameraPositionAndTarget_UpVecY(cameraPos, VGet(0, 0, 0));
    //SetupCamera_Perspective(Zoom);

    //モデルの座標指定
    DxLib::MV1SetPosition(model, charaPos);
    MV1SetRotationXYZ(model, VGet(0.0f, RotateY + M_PI, 0.0f));

    blink.PlayAnimation();

    m_StateManager->Doing();

    MV1PhysicsCalculation(model, 1000.0f / 60.0f);

    //モデルの描画
    DxLib::MV1DrawModel(model);

    //描画内容を画面に反映
    DxLib::ScreenFlip();

    return S_OK;
}

void DrawMMD::LoadModel()
{
    isDraw = false;

    MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
    MV1SetLoadModelPhysicsWorldGravity(-10.f);
    MV1InitModel();

    // TODO: 設定ファイル化
    MV1SetLoadModelAnimFilePath(L"../../C#/RealtimeListenMMD/Model/motion/motion");
    //model = MV1LoadModel(L"../../C#/RealtimeListenMMD/Model/ぽんぷ長式大和_水着/ぽんぷ長式大和＿水着mode.pmx");
    //model = MV1LoadModel(L"../../C#/RealtimeListenMMD/Model/Menace メナス(カノン改造)/メナス_edit.pmx");
    //model = MV1LoadModel(L"../../C#/RealtimeListenMMD/Model/アールビット式WF改変唯依姫(ｼｮｰﾄﾍｱ)/唯依姫(ｼｮｰﾄﾍｱ)1.00.pmx");
    model = MV1LoadModel(L"../../C#/RealtimeListenMMD/Model/タヌキとキツネ ver.1/キツネ.pmx");

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
