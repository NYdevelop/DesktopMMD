#include "MouseDrag.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "WinUtil.h"

MouseDrag::MouseDrag()
{
}

MouseDrag::~MouseDrag()
{
}

void MouseDrag::ButtonDown(unsigned char button)
{
    auto& buttonTuple = buttonMap[button];
    std::get<BUTTON_STATUS>(buttonTuple) = true;

    POINT pos;
    GetCursorPos(&pos);
    std::get<BUTTON_BEFORE_X>(buttonTuple) = pos.x;
    std::get<BUTTON_BEFORE_Y>(buttonTuple) = pos.y;

    std::get<BUTTON_DOWN_FUNC>(buttonTuple)(pos.x, pos.y);
}

void MouseDrag::Update()
{
    for(auto& i : buttonMap)
    {
        if (std::get<BUTTON_STATUS>(i.second) == false)
        {
            continue;
        }

        auto down = IsPress(i.first);
        std::get<BUTTON_STATUS>(i.second) = down;

        if (down == false)
        {
            continue;
        }

        POINT pos;
        GetCursorPos(&pos);
        std::get<BUTTON_DRAG_FUNC>(i.second)(pos.x - std::get<BUTTON_BEFORE_X>(i.second), pos.y - std::get<BUTTON_BEFORE_Y>(i.second));
        std::get<BUTTON_BEFORE_X>(i.second) = pos.x;
        std::get<BUTTON_BEFORE_Y>(i.second) = pos.y;
    }
}

void MouseDrag::SetButtonFunc(unsigned char button, const std::function<void(long, long)>& downFunc, const std::function<void(long, long)>& dragFunc)
{
    buttonMap[button] = std::tuple<bool, LONG, LONG, const std::function<void(LONG, LONG)>&, const std::function<void(LONG, LONG)>& >
        (false, 0L, 0L, downFunc, dragFunc);
}
