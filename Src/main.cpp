#include "MMD/ManageMMD.h"

using namespace std;

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー
int main()
{
    {
        ManageMMD manager;
        manager.Initialize();
        manager.Process();
        manager.Exit();
    }

    _CrtDumpMemoryLeaks();

    return 0;
}