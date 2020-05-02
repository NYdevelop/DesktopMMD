#include "UtilMMD.h"

void WalkStart(float screenX, float screenY, DrawMMD* mmd, WalkStateManager* walker)
{
    VECTOR ScreenPos = VGet(screenX, screenY, 0);
    ScreenPos.z = 0.0f;
    auto Start3DPos = ConvScreenPosToWorldPos(ScreenPos);

    ScreenPos.z = 1.0f;
    auto End3DPos = ConvScreenPosToWorldPos(ScreenPos);

    auto rayVec = VNorm(VSub(End3DPos, Start3DPos));
    auto pos = mmd->GetCharactorPos();
    if (abs(rayVec.y) < 0.5)
    {
        // 指定画面座標とrayを含む面、と、
        // キャラ座標から画面横方向に伸ばした線分の交点に移動

        // 画面横方向線分係数算出
        ScreenPos.x += 640.0f;
        ScreenPos.z = 0.0f;
        auto tmp3DPos = ConvScreenPosToWorldPos(ScreenPos);
        auto ScreenSlideVec = VNorm(VSub(tmp3DPos, Start3DPos));

        // 指定画面座標を含む面係数算出
        auto norm = VCross(rayVec, VGet(0.f, 1.f, 0.f));
        auto d = -VDot(norm, Start3DPos);

        auto t = d + VDot(norm, pos);
        auto newPos = VAdd(pos, VScale(ScreenSlideVec, t));
        walker->Start(newPos);
        return;
    }

    // ray線分とキャラy平面との交点に移動
    auto t = (pos.y - Start3DPos.y) / rayVec.y;
    auto newPos = VGet(Start3DPos.x + t * rayVec.x, pos.y, Start3DPos.z + t * rayVec.z);

    walker->Start(newPos);
}

float GetDistance(const VECTOR& v1, const VECTOR& v2)
{
    auto sub = VSub(v1, v2);
    return sqrt(VDot(sub, sub));
}
