#include "VideoTimeLineView.h"

LRESULT VideoTimeLineView::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    VideoTimeLineView* pThis = NULL;

    if (uMsg == WM_NCCREATE) {
        // WM_NCCREATE에서 lParam을 통해 this 포인터 설정
        LPCREATESTRUCT pCreate = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<VideoTimeLineView*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else {
        // 이후 메시지에서는 GWLP_USERDATA에서 this 포인터를 가져옴
        pThis = reinterpret_cast<VideoTimeLineView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis) {
        // 멤버 함수로 메시지 처리
        return pThis->HandleMessage(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

VideoTimeLineView::VideoTimeLineView(HWND hwnd)
{
    x = 0;
    y = 400;
    windowW = 400;
	windowH = 100;
    hVideoTimeLine = CreateChildWindow(
        hwnd,
        GetModuleHandle(nullptr),
        L"VideoView",
        WindowProc,
        x, y,
        windowW, windowH,
        this);

    CStatic* pPicture = (CStatic*)GetDlgItem(IDC_IMAGE); // IDC_IMAGE는 Picture Control ID
    HBITMAP hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MY_IMAGE), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

VideoTimeLineView::~VideoTimeLineView()
{
}

void VideoTimeLineView::OnResize(RECT& rect)
{
    int newX = rect.left;
    int newY = rect.top;
    int newW = rect.right - rect.left;
    int newH = rect.bottom - rect.top;
    OnResize(newX, newY, newW, newH);
}

void VideoTimeLineView::OnResize(int _x, int _y, int _w, int _h)
{
    x = _x;
    y = _y;
    windowW = _w;
    windowH = _h;
    MoveWindow(hVideoTimeLine, x, y, windowW, windowH, TRUE);
}

LRESULT VideoTimeLineView::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_LBUTTONDOWN:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    //case WM_NCHITTEST:
    //    break;
    //case WM_SIZING: 
    //    return TRUE;
    case WM_CHAR:
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}
