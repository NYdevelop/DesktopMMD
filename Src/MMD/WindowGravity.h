#pragma once

#include <utility>
#include "DxLib.h"

class WindowGravity
{
public:
    float PosUpdate(VECTOR pos);

private:
    bool CalcPos(int screenY, VECTOR originPos, VECTOR& updatePos, bool isDesktop = false);

    // first:  ���ʏ�̓_
    // second: �@��
    std::pair<VECTOR, VECTOR> GetPlane(float screenY);

    bool CheckOtherWindow(VECTOR originPos, VECTOR& updatePos);

    float m_Gravity = 1.f;
};