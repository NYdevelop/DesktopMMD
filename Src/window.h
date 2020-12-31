#pragma once
#include <windows.h>
#include <functional>
#include <map>
#include <vector>
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
    void SetCallbackMsg(UINT msg, const std::function<void(WPARAM, LPARAM)>& func);

    void InitContextMenu(const std::vector<std::tuple<HMENU, ULONG, UINT, std::wstring>>& config);

private:
    HRESULT InitWindow(HWND* hWnd);

    void Draw(HDC hDc);

    // �E�B���h�E�v���V�[�W��
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
    std::function<void(WPARAM, LPARAM)> m_CommandCallback;
    std::function<void(WPARAM, LPARAM)> m_WheelCallback;
    std::function<void()> m_MButtonCallback;
    std::map<UINT, std::function<void(WPARAM, LPARAM)>> m_CallbackMap;
};