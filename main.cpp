#include <thread>
#include <iostream>

//#include "window.h"
//#include "DrawMMD.h"
#include "ManageMMD.h"

using namespace std;

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー
int main()
{
    //thread t([]()
    //{

    //});
    ManageMMD manager;
    manager.Process();
    manager.Exit();

    _CrtDumpMemoryLeaks();
    //t.join();

    return 0;
}


//int main()
//{
//    DrawMMD mmd;
//
//    mmd.preInitialize();
//    if (DxLib::DxLib_Init() == -1)return -1;
//    mmd.afterInitialize();
//
//    // メインスレッドでのループでないと入力取得できない
//    const std::chrono::microseconds INTERVAL(1000 * 1000 / 30);
//    while (DxLib::ProcessMessage() == 0)
//    {
//        auto start = std::chrono::system_clock::now();
//
//        if (mmd.mainProcess() != 0)
//        {
//            break;
//        }
//
//        auto end = std::chrono::system_clock::now();
//        auto elapsed = end - start;
//
//        // 毎回、インターバル時間のスリープを保障
//        if (elapsed < INTERVAL) {
//            std::this_thread::sleep_for(INTERVAL - elapsed);
//        }
//    }
//
//    DxLib::DxLib_End(); // ＤＸライブラリ使用の終了処理
//    return 0; // ソフトの終了 
//}