#pragma once
#include <windows.h>
#include <functional>
#include "UtilTimer.h"

class CWindow {
public:
    CWindow();
    ~CWindow();

    HRESULT Init();
    HRESULT Process(int fps = 30);

    HWND GetHWnd();
    bool IsClose();

    void SetDrawFunc(const std::function<void(HDC)>& func);

private:
    HRESULT InitWindow(HWND* hWnd);

    void Draw(HDC hDc);

    // ウィンドウプロシージャ
    static LRESULT CALLBACK WindProc(
        HWND hwnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam);

    UtilTimer m_Timer;
    HBRUSH m_BackGround;

    HWND                  m_hWnd;
    BOOL                  m_bInitDone;
    int                   m_nWidth;
    int                   m_nHeight;

    HMENU m_ContextMenu;

    std::function<void(HDC)> m_DrawFunc;
};