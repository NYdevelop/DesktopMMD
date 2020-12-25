#include "WaitState.h"

#include <random>
#include "Util/UtilMMD.h"

static std::mt19937 mt;

void WaitState::Initialize()
{
    std::cout << "state: wait" << std::endl;

    /// カメラ方向を向く
    m_mmd->canViewCamera = true;
}

void WaitState::Doing()
{
    if (m_RandomMove == false) return;
    if (walkManager->IsMove() == true) return;

    if (mt() % 500 != 0) return;

    // ランダムなスクリーン座標へ移動開始
    std::uniform_real_distribution<> randRange(0.967598021, 0.99);
    auto z = randRange(mt);
    WalkStart(
        VGet((float)(mt() % dispWidth), (float)(mt() % dispHeight), z),
        m_mmd.get(),
        walkManager);
}

void WaitState::End()
{
    m_mmd->canViewCamera = false;
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
