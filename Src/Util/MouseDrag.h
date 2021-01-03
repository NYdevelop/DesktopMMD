#pragma once

#include <map>
#include <functional>

class MouseDrag
{
public:
    MouseDrag();
    ~MouseDrag();

    void ButtonDown(unsigned char button);

    void Update();

    void SetButtonFunc(unsigned char button, const std::function<void(long, long)>& downFunc, const std::function<void(long, long)>& dragFunc);

private:
    enum ButtonMapMap : int
    {
        BUTTON_STATUS = 0,
        BUTTON_BEFORE_X,
        BUTTON_BEFORE_Y,
        BUTTON_DOWN_FUNC,
        BUTTON_DRAG_FUNC,
    };

    std::map<unsigned char, std::tuple<bool, long, long, std::function<void(long, long)>, std::function<void(long, long)> > > buttonMap;
};

