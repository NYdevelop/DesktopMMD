#include "MMD/DrawMMD.h"

#include "Util/WinUtil.h"
#include "Util/UtilMMD.h"
#include <fstream>

using namespace std;

DrawMMD::DrawMMD(const std::string & animPath, const std::string & modelPath,
    const float charaX, const float charaY, const float charaZ,
    const float charaDirect,
    const float cameraX, const float cameraY, const float cameraZ)
{
    m_AnimPath = animPath;
    m_ModelPath = modelPath;

    //DxLib_Init以前で呼び出すべき処理
    RECT rc;
    GetWindowRect(GetDesktopWindow(), &rc);
    dispWidth = rc.right - rc.left;
    dispHeight = rc.bottom - rc.top;


    RotateY = charaDirect;
    m_Zoom = cameraZ * -1.f;

    charaPos = VGet(charaX, charaY, charaZ);
    cameraPos = VGet(cameraX, cameraY, m_Zoom * -1.f);

    SetUseDirect3DVersion(DX_DIRECT3D_9EX);
}

void DrawMMD::afterInitialize()
{
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//背景を設定(透過させる)

    DxLib::SetDrawScreen(DX_SCREEN_BACK);//描画対象を裏画面にする

    cameraViewOffset = VGet(0.f, 7.f, 1.f);
    cameraViewPos = VGet(0.f, 0.f, 0.f);

    SetupCamera_Perspective(30.f * DX_PI_F / 180.f);
    SetCameraNearFar(1.f, 1000.0f);
    SetCameraPositionAndTarget_UpVecY(cameraPos, cameraViewPos);

    SetFullSceneAntiAliasingMode(4, 4);
    // gHandle = MakeGraph(1920, 1040);

    //ChangeLightTypePoint(
    //    VGet(0.0f, 0.0f, 0.0f),
    //    1000.f,
    //    1.f,
    //    0.f,
    //    0.f);

    // プログラマブルシェーダーモデル２．０が使用できない場合はエラーを表示して終了
    if (GetValidShaderVersion() < 300)
    {
        // エラー表示
        std::cout << "プログラマブルシェーダー２．０が使用できない環境のようです" << std::endl;

        // キー入力待ち
        WaitKey();

        // ＤＸライブラリの後始末
        DxLib_End();

        // ソフト終了
        return;
    }

    // 頂点シェーダーを読み込む
    VertexShaderHandle = LoadVertexShader(L"Src/Shader/SkinMesh4_DirLight_ToonVS.vso");
    std::cout << "VShader handle: " << VertexShaderHandle << std::endl;

    // ピクセルシェーダーを読み込む
    PixelShaderHandle = LoadPixelShader(L"Src/Shader/SkinMesh4_DirLight_ToonPS.pso");
    std::cout << "PShader handle: " << PixelShaderHandle << std::endl;

    // 使用する頂点シェーダーをセット
    SetUseVertexShader(VertexShaderHandle);

    // 使用するピクセルシェーダーをセット
    SetUsePixelShader(PixelShaderHandle);

    MV1SetUseOrigShader(TRUE);
}

//メインとなる処理
int DrawMMD::mainProcess()
{
    if (isDraw == false) return S_FALSE;

    //描画内容を全削除
    DxLib::ClearDrawScreen();
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//背景を設定(透過させる)

    SetCameraPositionAndTarget_UpVecY(
        cameraPos, VAdd(cameraViewPos, cameraViewOffset));

    auto rayVec = GetRayVec();

    // TODO: 設定ファイルからの落下ON/OFF
    // 歩き中は落下しない
    //if (m_StateManager->GetCurrentStateIndex() != EState::STATE_WALK &&
    //    abs(atan2(rayVec.z, rayVec.y) + DX_PI_F / 2) < DX_PI_F / 4)
    //{
    //    charaPos.y = gravity.PosUpdate(charaPos);
    //}

    DxLib::MV1SetPosition(model, charaPos);
    MV1SetRotationXYZ(model, VGet(0.0f, RotateY + DX_PI_F, 0.0f));

    if (canViewCamera)
    {
        ViewCamera(
            VSub(MV1GetFramePosition(model, boneHead), rayVec) ,
            MTranspose(MV1GetFrameLocalWorldMatrix(model, boneHead)),
            defHeadLocalRot, model, boneHead);
    }
    else
    {
        MV1ResetFrameUserLocalMatrix(model, boneHead);
    }

    m_StateManager->Doing();

    MV1PhysicsCalculation(model, 1000.0f / 60.0f);

    //モデルの描画
    DxLib::MV1DrawModel(model);

    //GetDrawScreenGraph(0,0,1920,1040, gHandle);
    //GraphFilter(gHandle, DX_GRAPH_FILTER_GAUSS, 8, 50);
    //DrawGraph(0,0,gHandle, FALSE);

    //描画内容を画面に反映
    DxLib::ScreenFlip();

    return S_OK;
}

void Write(std::ofstream& ofs, const std::string& keyWord, const std::string& value)
{
    ofs << keyWord << " = " << value << std::endl;
};

void DrawMMD::Exit()
{
    isDraw = false;
    MV1DeleteModel(model);

    // 読み込んだ頂点シェーダーの削除
    DeleteShader(VertexShaderHandle);

    // 読み込んだピクセルシェーダーの削除
    DeleteShader(PixelShaderHandle);

    std::ofstream ofs("config_pos.txt");
    Write(ofs, "CHARA_POS_X", to_string(charaPos.x));
    Write(ofs, "CHARA_POS_Y", to_string(charaPos.y));
    Write(ofs, "CHARA_POS_Z", to_string(charaPos.z));

    Write(ofs, "CHARA_DIRECT", to_string(RotateY));

    Write(ofs, "CAMERA_POS_X", to_string(cameraPos.x));
    Write(ofs, "CAMERA_POS_Y", to_string(cameraPos.y));
    Write(ofs, "CAMERA_POS_Z", to_string(cameraPos.z));
}

void DrawMMD::LoadModel()
{
    isDraw = false;

    MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
    // MV1SetLoadModelPhysicsWorldGravity(-10.f);
    MV1InitModel();

    TCHAR buf[255];
    MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, m_AnimPath.c_str(), -1, buf, 255);
    MV1SetLoadModelAnimFilePath(buf);

    MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, m_ModelPath.c_str(), -1, buf, 255);
    std::cout << "load start..." << std::endl;
    model = MV1LoadModel(buf);

    std::cout << "load finish." << std::endl;
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

    boneHead = MV1SearchFrame(model, L"頭");
    defHeadLocalRot = MV1GetFrameLocalMatrix(model, boneHead);


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

void DrawMMD::SetEnableLight(bool enable)
{
    auto val = enable ? TRUE : FALSE;
    SetUseLighting(val);
}
