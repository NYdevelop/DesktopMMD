#include "DrawMMD.h"

#include "WinUtil.h"
//#include <string>
#include <iostream>

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

    //文字サイズの設定
    //SetFontSize(32);

    //ウィンドウを最前面に表示する
    // http://dixq.net/forum/viewtopic.php?f=3&t=10766
    HWND hWnd = GetMainWindowHandle();
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    DxLib::SetDrawScreen(DX_SCREEN_BACK);//描画対象を裏画面にする

    // TODO: 設定ファイル化
    MV1SetLoadModelAnimFilePath(L"../../C#/RealtimeListenMMD/Model/motion/motion");
    model = MV1LoadModel(L"../../C#/RealtimeListenMMD/Model/結月ゆかり_純_ver1.0/結月ゆかり_純.pmd");//モデルデータの読み込み
                                                                                             //アニメーション設定
                                                                                             // http://dixq.net/g/3d_01.html

    if (model == -1)
    {
        throw "model load error";
    }
    if (MV1GetAnimNum(model) == 0)
    {
        throw "motion load error";
    }

    blink.AttachAnime(model, 2);
    blink.SetMaximumTime(250.f);

    unazuki.AttachAnime(model, 3);
    unazuki.SetPlaySpeed(BASE_PLAY_SPEED);
    cout << BASE_PLAY_SPEED << endl;
    m_Bpm = BASE_BPM;

    auto noSoundAnimeIndex = 1;
    noSoundAnimeIndex = MV1AttachAnim(model, noSoundAnimeIndex, -1, FALSE);//モーションの選択

    stableAnimIndex = 4;
    stableAnimIndex = MV1AttachAnim(model, stableAnimIndex, -1, FALSE);//モーションの選択

    unazuki2NoSound.SetAnim(unazuki.GetAnimIndex(), noSoundAnimeIndex, model);
    noSound2Unazuki.SetAnim(noSoundAnimeIndex, unazuki.GetAnimIndex(), model);
    noSound2Stable.SetAnim(noSoundAnimeIndex, stableAnimIndex, model);

    MV1SetAttachAnimTime(model, noSoundAnimeIndex, 0);//モーションの再生位置を設定
    MV1SetAttachAnimBlendRate(model, noSoundAnimeIndex, 0);

    MV1SetAttachAnimTime(model, stableAnimIndex, 0);//モーションの再生位置を設定
    MV1SetAttachAnimBlendRate(model, stableAnimIndex, 0);

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

//メインとなる処理
int DrawMMD::mainProcess()
{
    //描画内容を全削除
    DxLib::ClearDrawScreen();
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//背景を設定(透過させる)

    //モデルの座標指定
    DxLib::MV1SetPosition(model, charaPos);

    // なめらかなアニメーション変化
    unazuki2NoSound.Transrate();
    noSound2Unazuki.Transrate();
    noSound2Stable.Transrate();

    blink.PlayAnimation();
    if (m_IsNoSound == false)
    {
        unazuki.PlayAnimation();
    }

    //モデルの描画
    DxLib::MV1DrawModel(model);

    //描画内容を画面に反映
    DxLib::ScreenFlip();

    return S_OK;
}

void DrawMMD::SetBPM(float bpm)
{
    cout << bpm << ": ";
    if (bpm > 140) bpm /= 2.0f;
    cout << bpm << endl;
    unazuki.SetPlaySpeed(BASE_PLAY_SPEED * bpm / BASE_BPM);
    m_Bpm = bpm;
}

void DrawMMD::SetNoSound(bool isNoSound)
{
    if (m_IsNoSound == isNoSound) return;

    if (isNoSound == true)
    {
        unazuki2NoSound.SetTransTime(20);
    }
    else
    {
        noSound2Unazuki.SetTransTime(10);
        noSound2Stable.SetTransTime(10);
    }
    m_IsNoSound = isNoSound;
}

void DrawMMD::SetMoveAttitude(double rate)
{
    MV1SetAttachAnimBlendRate(model, unazuki.GetAnimIndex(), rate);
    MV1SetAttachAnimBlendRate(model, stableAnimIndex, 1 - rate);
}
