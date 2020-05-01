#include "MMD/ManageMMD.h"

using namespace std;

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー
int main()
{
    // TODO: カメラの方を向く
    // TODO: 指定座標に向かって自動で移動(画面座標)
    {
        ManageMMD manager;
        manager.Initialize();
        manager.Process();
        manager.Exit();
    }

    _CrtDumpMemoryLeaks();

    return 0;
}