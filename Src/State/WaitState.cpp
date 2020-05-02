#include "WaitState.h"

#include <random>
#include "Util/UtilMMD.h"

static std::mt19937 mt;
void WaitState::Doing()
{
    if (m_RandomMove == false) return;
    if (walkManager->IsMove() == true) return;

    if (mt() % 500 != 0) return;

    // ランダムなスクリーン座標へ移動開始
    WalkStart((float)(mt() % dispWidth), (float)(mt() % dispHeight), m_mmd.get(), walkManager);
}

void WaitState::OnceInitial()
{
    RECT rc;
    GetWindowRect(GetDesktopWindow(), &rc);
    dispWidth = rc.right - rc.left;
    dispHeight = rc.bottom - rc.top;

    std::random_device rnd;     // 非決定的な乱数生成器
    mt = move(std::mt19937(rnd()));            // メルセンヌ・ツイスタの32ビット版、引数は初期シード
}
