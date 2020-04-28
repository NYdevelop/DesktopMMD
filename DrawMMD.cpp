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

    //解像度，ウィンドウモード指定
    //DxLib::ChangeWindowMode(TRUE); // ウィンドウモードに変更
    //DxLib::SetGraphMode(dispWidth, dispHeight, 32); // 解像度の設定

    //http://www.sys-cube.co.jp/blog/5347.html
    //DxLib::SetWindowStyleMode(2); // ウィンドウのスタイルを枠無しに指定

    // 重たい処理
    //DxLib::SetUseBackBufferTransColorFlag(TRUE); // ウィンドウを透明に指定
}

void DrawMMD::afterInitialize()
{
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//背景を設定(透過させる)

    //3D周りの初期化
    //DxLib::SetUseZBuffer3D(TRUE);
    //DxLib::SetWriteZBuffer3D(TRUE);

    //文字サイズの設定
    //SetFontSize(32);

    //ウィンドウを最前面に表示する
    // http://dixq.net/forum/viewtopic.php?f=3&t=10766
    HWND hWnd = GetMainWindowHandle();
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    //DxLib::SetAlwaysRunFlag(TRUE);//ウィンドウがアクティブ状態でなくても実行する
    DxLib::SetDrawScreen(DX_SCREEN_BACK);//描画対象を裏画面にする

    MV1SetLoadModelAnimFilePath(L"../C#/RealtimeListenMMD/Model/motion/motion");
    model = MV1LoadModel(L"../C#/RealtimeListenMMD/Model/結月ゆかり_純_ver1.0/結月ゆかり_純.pmd");//モデルデータの読み込み
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

    attachIndex = DxLib::MV1AttachAnim(model, 3, -1, FALSE);
    totalTime = DxLib::MV1GetAttachAnimTotalTime(model, attachIndex);
    playTime = 0.0f;
    playSpeed = BASE_PLAY_SPEED;
    cout << BASE_PLAY_SPEED << endl;
    m_Bpm = BASE_BPM;

    //輪郭線の大きさを修正する
    //通常MV1SetScaleで拡大すると，輪郭線の大きさも拡大されてしまう
    //全ての輪郭線にアクセスし，MV1SetScaleで拡大した値で割ることで，太さをもとに戻す
    // http://peroon.hatenablog.com/entry/20110402/1301773655
    //int MaterialNum = DxLib::MV1GetMaterialNum(model);
    //for (int i = 0; i < MaterialNum; i++)
    //{
    //    // マテリアルの輪郭線の太さを取得 
    //    float dotwidth = DxLib::MV1GetMaterialOutLineDotWidth(model, i);
    //    // マテリアルの輪郭線の太さを拡大した分小さくする 
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

//メインとなる処理
int DrawMMD::mainProcess()
{
    //描画内容を全削除
    DxLib::ClearDrawScreen();
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//背景を設定(透過させる)

    //モデルの移動
    //const float speed = 0.1f;
    //if (DxLib::CheckHitKey(KEY_INPUT_RIGHT))charaPos.x += speed;
    //if (DxLib::CheckHitKey(KEY_INPUT_LEFT))charaPos.x -= speed;

    //モデルの座標指定
    DxLib::MV1SetPosition(model, charaPos);

    //モデルのアニメーション設定
    //http://dixq.net/g/3d_01.html
    // 再生時間を進める
    // ↓の1.0はいい感じに動くように適当に決める
    // 環境によって処理落ちなどで実行速度が変わることがある
    playTime += playSpeed;
    // 再生時間がアニメーションの総再生時間に達したら再生時間を０に戻す
    if (playTime >= totalTime) {
        playTime = 0.0f;
    }
    // 再生時間をセットする
    DxLib::MV1SetAttachAnimTime(model, attachIndex, playTime);

    //モデルの描画
    DxLib::MV1DrawModel(model);

    //DrawString(
    //    1750, 1000,
    //    StringToWString(std::to_string(m_Bpm)).c_str(),
    //    GetColor(0, 0, 0));

    //クリックに反応
    //if (DxLib::GetMouseInput()&MOUSE_INPUT_LEFT)
    //{
    //    //キャラクターの「スクリーン上」での座標を取得する
    //    DxLib::VECTOR pos = DxLib::ConvWorldPosToScreenPos(charaPos);
    //    DxLib::DrawFormatString(pos.x, pos.y-600, 0x000000, L">>クリックされてるよ～");
    //}

    //描画内容を画面に反映
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
