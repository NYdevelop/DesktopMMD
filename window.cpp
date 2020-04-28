#include "window.h"

//#include <wingdi.h>

#include <iostream>

#include <string>
#include <vector>

#include "Define.h"

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

using namespace std;

COLORREF BACKGROUND = RGB(1, 1, 1);

HRESULT CWindow::InitWindow(HWND* hWnd) {
    const TCHAR kszWindowTitle[] = TEXT("window title");
    const TCHAR kszWindowClass[] = TEXT("window Class");

    WNDCLASSEX sWndClassEx = { 0 };
    sWndClassEx.cbSize = sizeof(WNDCLASSEX);
    sWndClassEx.style = CS_HREDRAW | CS_VREDRAW;
    sWndClassEx.lpfnWndProc = (WNDPROC)WindProc;
    sWndClassEx.cbClsExtra = 0;
    sWndClassEx.cbWndExtra = 0;
    sWndClassEx.hInstance = GetModuleHandle(NULL);
    sWndClassEx.hIcon = NULL;// LoadIcon(sWndClassEx.hInstance, (LPCTSTR)IDI_TESTSHARESURFACE);
    sWndClassEx.hIconSm = NULL;// LoadIcon(sWndClassEx.hInstance, (LPCTSTR)IDI_SMALL);
    sWndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
    sWndClassEx.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH),//(HBRUSH)(COLOR_WINDOW + 1);
        sWndClassEx.lpszMenuName = NULL;// (LPCSTR)IDC_TESTSHARESURFACE;
    sWndClassEx.lpszClassName = kszWindowClass;

    if (!RegisterClassEx(&sWndClassEx))
        return E_FAIL;

    HWND hTmpWnd = CreateWindowEx(
        WS_EX_LAYERED,
        kszWindowClass,
        kszWindowTitle,          // window title
        WS_POPUP | WS_VISIBLE,     // window style WS_OVERLAPPEDWINDOW
        CW_USEDEFAULT,           // x
        0,                       // y
        1920,//CW_USEDEFAULT,           // width
        1040,                       // height
        NULL,                    // parent
        NULL,                    // menu
        sWndClassEx.hInstance,   // instance
        NULL);

    ::SetProp(hTmpWnd, TEXT("THIS_INSTANCE"), this);

    SetWindowPos(hTmpWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    SetLayeredWindowAttributes(
        hTmpWnd,            // ウィンドウのハンドル
        BACKGROUND,         // COLORREF値
        0,                  // アルファの値
        LWA_COLORKEY        // アクションフラグ
    );

    *hWnd = hTmpWnd;
    if (!hTmpWnd)
        return E_FAIL;

    ShowWindow(
        hTmpWnd,           // window handle
        SW_SHOWDEFAULT);   // 
    UpdateWindow(hTmpWnd);

    /// コンテキストメニュー作成
    m_ContextMenu = CreatePopupMenu();
    m_ModeMenu = CreatePopupMenu();

    /* メニュー項目追加 */
    AppendMenu(m_ContextMenu, MF_POPUP, (UINT)m_ModeMenu, L"Mode");
    AppendMenu(m_ContextMenu, MF_STRING, CONTEXT_EXIT,    L"Exit");

    AppendMenu(m_ModeMenu, MF_STRING, CONTEXT_MODE_WAIT,   L"Wait");
    AppendMenu(m_ModeMenu, MF_STRING, CONTEXT_MODE_RHYTHM, L"Rhythm");
    AppendMenu(m_ModeMenu, MF_STRING, CONTEXT_MODE_READ,   L"Read");

    return S_OK;
}


LRESULT CALLBACK CWindow::WindProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam) {


    switch (uMsg) {

    case WM_COMMAND:
    {
        CWindow* win = (CWindow*)::GetProp(hwnd, TEXT("THIS_INSTANCE"));
        if (win->m_CommandCallback != nullptr)
        {
            win->m_CommandCallback(wParam, lParam);
        }
    }
    break;


    case WM_DESTROY:
    {
        CWindow* win = (CWindow*)::GetProp(hwnd, TEXT("THIS_INSTANCE"));
        win->m_Timer.Break_Force();

        //_CrtDumpMemoryLeaks();
        ::PostQuitMessage(0);
    }
    break;

    case WM_RBUTTONDOWN:
    {
        POINT pt;
        /* マウスカーソル座標取得 */
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);

        /* スクリーン座標に変換 */
        ClientToScreen(hwnd, &pt);

        /* カーソル位置にメニューを出す */
        CWindow* win = (CWindow*)::GetProp(hwnd, TEXT("THIS_INSTANCE"));
        TrackPopupMenu(win->m_ContextMenu, 0, pt.x, pt.y, 0, hwnd, NULL);
    }
    break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}



CWindow::CWindow() :
    m_BackGround(CreateSolidBrush(BACKGROUND))
{
    m_hWnd = NULL;

    m_bInitDone = FALSE;

    m_nWidth = 0;
    m_nHeight = 0;
}

CWindow::~CWindow()
{
    DeleteObject(m_BackGround);
}

HRESULT CWindow::Init()
{
    HRESULT hResult = E_FAIL;
    if (m_bInitDone) return S_OK;
    hResult = InitWindow(&m_hWnd);

    if (SUCCEEDED(hResult))
    {
        m_bInitDone = TRUE;
    }
    return hResult;
}

HRESULT CWindow::Process(int fps)
{
    HRESULT hResult = E_FAIL;
    if (!m_bInitDone)
    {
        MessageBox(NULL, TEXT("window not initalize"), NULL, MB_ICONERROR);
        return hResult;
    }

    m_Timer.Start([=]()
    {
        //InvalidateRect(m_hWnd, NULL, FALSE);
        m_DrawFunc(nullptr);
        return true;
    },
        1000 * 1000 / fps);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return hResult;
}

HWND CWindow::GetHWnd()
{
    return m_hWnd;
}

bool CWindow::IsClose()
{
    return !m_Timer.IsRunning();
}

void CWindow::SetDrawFunc(const function<void(HDC)>& func)
{
    m_DrawFunc = func;
}

void CWindow::SetCallbackCommand(const std::function<void(WPARAM, LPARAM)>& func)
{
    m_CommandCallback = func;
}

void CWindow::Draw(HDC hDc)
{
    if (m_DrawFunc != nullptr)
    {
        m_DrawFunc(hDc);
    }
}
