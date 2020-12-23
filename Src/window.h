#pragma once
#include <windows.h>
#include <functional>
#include "Util/UtilTimer.h"

class CWindow {
public:
    CWindow();
    ~CWindow();

    HRESULT Init();
    HRESULT Process(int fps = 30);
    void Close();

    HWND GetHWnd();
    HMENU GetContextMenu();
    bool IsClose();

    void SetDrawFunc(const std::function<void(HDC)>& func);
    void SetCallbackCommand(const std::function<void(WPARAM, LPARAM)>& func);
    void SetCallbackWheel(const std::function<void(WPARAM, LPARAM)>& func);

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
    HMENU m_ModeMenu;

    std::function<void(HDC)> m_DrawFunc;
    std::function<void(WPARAM, LPARAM)> m_CommandCallback;
    std::function<void(WPARAM, LPARAM)> m_WheelCallback;
};