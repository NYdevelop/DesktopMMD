#include "WindowGravity.h"

#include "Util/UtilMMD.h"

float WindowGravity::PosUpdate(VECTOR pos)
{
    m_Gravity = 0.01f;
    auto calcPos = pos;
    calcPos.y -= m_Gravity;

    // �قȂ�E�B���h�E�Ƃ̐ڐG����
    if (CheckOtherWindow(pos, calcPos) == true)
    {
        return calcPos.y;
    }

    /// ��ʉ��Ƃ̐ڐG����
    CalcPos(1040, pos, calcPos, true);
    return calcPos.y;
}

bool WindowGravity::CalcPos(int screenY, VECTOR originPos, VECTOR& updatePos, bool isDesktop)
{
    auto plane = GetPlane(static_cast<float>(screenY));
    auto t = VDot(plane.second, VSub(originPos, plane.first));

    // ���������K�v������
    if ((isDesktop && t > 0) || // ������ւ̕␳
        IsHitPlane(originPos, updatePos, plane.first, plane.second)) // �����蔻��
    {
        updatePos.y = originPos.y + t;
        return true;
    }

    return false;
}

std::pair<VECTOR, VECTOR> WindowGravity::GetPlane(float screenY)
{
    auto pos0 = ConvScreenPosToWorldPos(VGet(0, screenY, 0));
    auto posX = ConvScreenPosToWorldPos(VGet(600, screenY, 0));
    auto posZ = ConvScreenPosToWorldPos(VGet(0, screenY, 1));
    auto norm = VNorm(VCross(VSub(posX, pos0), VSub(posZ, pos0)));
    return std::pair<VECTOR, VECTOR>(pos0, norm);
}

bool WindowGravity::CheckOtherWindow(VECTOR originPos, VECTOR& updatePos)
{
    HWND hWnd = GetWindow(GetDesktopWindow(), GW_CHILD);

    auto charaScreenPos = ConvWorldPosToScreenPos(originPos);

    // TODO: ��ʂɕ\������Ă���E�B���h�E�̂ݔ���
    // ���݂́AZ�I�[�_�[���ɍŏ����܂��͍ő剻�E�B���h�E�܂Ŕ���

    RECT rect;
    WINDOWINFO info = { 0 };
    GetWindowInfo(hWnd, &info);
    while (hWnd != nullptr)
    {
        if ((info.dwStyle & WS_VISIBLE) > 0 && (info.dwStyle & WS_CAPTION) > 0)
        {

            if ((info.dwStyle & WS_MINIMIZE) > 0 || (info.dwStyle & WS_MAXIMIZE) > 0)
            {
                break;
            }

            TCHAR title[255];
            GetWindowRect(hWnd, &rect);
            GetWindowText(hWnd, title, 255);
//            cout << WStringToString(title) << endl;;
            if ((charaScreenPos.x > rect.left && charaScreenPos.x < rect.right) &&
                 CalcPos(rect.top, originPos, updatePos) == true)
            {
                return true;
            }

        }

        hWnd = GetWindow(hWnd, GW_HWNDNEXT);
        GetWindowInfo(hWnd, &info);
    }
    return false;
}
