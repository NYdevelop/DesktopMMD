#include "UtilTimer.h"

#include <iostream>

void UtilTimer::Start(std::function<bool(void)> func, unsigned long microsecond)
{
    m_alive = true;
    m_Func = func;
    m_thread1 = std::thread([=] {
        // 処理と処理の間を n 秒以上空ける。（毎回 n 秒間隔保障なので、n 秒毎に定期的にスタートされるという意味ではない）
        const std::chrono::microseconds interval(microsecond);
        while (this->m_alive) {

            auto start = std::chrono::system_clock::now();

            // 処理が書かれたメソッドを呼び出す
            if (m_Func() == false)
            {
                break;
            }

            auto end = std::chrono::system_clock::now();
            auto elapsed = end - start;
            // std::cout << "elapsed.count()=[" << elapsed.count() << "]" << std::endl;

            // 毎回、インターバル時間のスリープを保障
            if (elapsed < interval)
            {
                // std::cout << (interval - elapsed).count()/ 10.0 / 1000 << std::endl;
                std::this_thread::sleep_for(interval - elapsed);
            }
        }
        this->m_alive = false;
        std::cout << "finished." << std::endl;
    });
}

void UtilTimer::Block()
{
    // スタートさせたスレッドが終了するまで、これ以上進まない。
    m_thread1.join();
    // スレッドを止めるか、勝手に止まるかすると　ここから以下に進む。
}

void UtilTimer::Break_Force()
{
    if (m_alive == false)
    {
        return;
    }

    // スレッドを止める
    m_thread1.detach();

    // スレッドの中のループから抜けさせる
    this->m_alive = false;
}

bool UtilTimer::IsRunning()
{
    return m_alive;
}

void UtilTimer::Sleep_Milliseconds(long milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}