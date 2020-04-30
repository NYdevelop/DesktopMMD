#pragma once

#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>

/**
 * スレッドを立て、テキストファイルの内容が更新されていないか確認します。
 * 参照 : 「c++で一定間隔で関数を実行させたい」 (teratail) https://teratail.com/questions/27449
 * 参照 : 「std::thread::thread」 (C++言語の入門サイトです) http://kaworu.jpn.org/cpp/std::thread::thread
 * 参照 : 「std::this thread::sleep for」 (C++言語の入門サイトです) http://kaworu.jpn.org/cpp/std::this_thread::sleep_for
 * 参照 : 「C++1z ラムダ式での*thisのコピーキャプチャ」 (Faith and Brave - C++で遊ぼう) http://faithandbrave.hateblo.jp/entry/2016/04/27/191209
 * 参照 : 「C++ で継承したときにサブクラスのメンバ関数を呼ぶためには virtual をつけて仮想関数にする」 (sotarokのお勉強) http://d.hatena.ne.jp/strkpy/20100401/1270133069
 */
class UtilTimer
{
public:
    /**
     * スレッドを開始します。
     */
    void Start(std::function<bool(void)> func, unsigned long microsecond);
    /**
     * スタートさせたスレッドが動いている間は、処理を先に進ませません。
     */
    void Block();
    /**
     * 処理の途中でも、強制的にスレッドを終了します。
     */
    void Break_Force();

    bool IsRunning();

    /**
     * （このクラスの持つスレッドではなく）この関数を呼び出したスレッドを、指定ミリ秒停止させます。
     * ネームスペースを切るのがめんどくさいので　このクラスに置いている。
     */
    static void Sleep_Milliseconds(long milliseconds);

protected:
    std::thread m_thread1;
    /**
     * スレッドを停止しても、処理のループは動き続けているようだったので、
     * このフラグを偽にすることで、スレッドの中のループから抜けさせます
     */
    bool m_alive = false;
    /**
     * 別スレッドで実行したい処理を書いたメソッドを持っています。
     */
    std::function<bool(void)> m_Func;
};