#include "WaitState.h"

#include <random>

static std::mt19937 mt;
void WaitState::Doing()
{
    if (m_RandomMove == false) return;
    if (walkManager->IsMove() == true) return;

    if (mt() % 500 != 0) return;

    // ランダムワールド座標算出
    auto ScreenPos = VGet(0.f, 0.f, 0.f);
    ScreenPos.x = (float)(mt() % dispWidth);
    ScreenPos.y = (float)(mt() % dispHeight);

    ScreenPos.z = 0.0f;
    auto Start3DPos = ConvScreenPosToWorldPos(ScreenPos);

    ScreenPos.z = 1.0f;
    auto End3DPos = ConvScreenPosToWorldPos(ScreenPos);

    auto rayVec = VNorm(VSub(End3DPos, Start3DPos));
    if (abs(rayVec.y) < 0.5) return;

    auto pos = m_mmd->GetCharactorPos();
    auto t = (pos.y - Start3DPos.y) / rayVec.y;
    auto newPos = VGet(Start3DPos.x + t * rayVec.x, pos.y, Start3DPos.z + t * rayVec.z);
    walkManager->Start(newPos);
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
