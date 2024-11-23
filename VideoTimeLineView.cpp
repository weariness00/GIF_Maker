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

VideoTimeLineView::VideoTimeLineView(HWND hwnd, RECT rect)
{
    hVideoTimeLine = CreateChildWindow(
        hwnd,
        GetModuleHandle(nullptr),
        L"VideoTimeLineView",
        WindowProc,
        rect,
        this,
        RGB(200,200,200));
}

VideoTimeLineView::~VideoTimeLineView()
{
}

void VideoTimeLineView::OnResize(RECT& rect)
{
    wTransform.SetRect(rect);
    auto wRect = *wTransform.GetWorldRect();
    // x,y 위치에서 w,h 크기만큼 그려주도록 변환
    int x = wRect.left;
    int y = wRect.top;
    int width = wRect.right + wRect.left;
    int height = wRect.bottom + wRect.top;

    MoveWindow(hVideoTimeLine, x, y, width, height, TRUE);
}

void VideoTimeLineView::OnResize(int _x, int _y, int _w, int _h)
{
    RECT r{ _x,_y,_w,_h };
    OnResize(r);
}

LRESULT VideoTimeLineView::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CREATE:{
        RECT r;
        GetClientRect(hVideoTimeLine, &r);
        GetClientRect(hWnd, &r);
        dbWindow = new DoubleBufferingWindow(hWnd);
        auto dbMemHDC = dbWindow->GetMemHDC();
        backgroundImage.CreateImage(dbMemHDC, backgroundImagePath);
        backgroundImage.wTransform.SetRect({ 0, 0, 1000, 1000 });
        SetChild(&backgroundImage);
        for (int i = 0; i < 1; i++)
        {
            ImageController* lineImage = new ImageController();
            lineImage->wTransform.SetRect({ i * 10, 0, 5, 2000 });
            SetChild(lineImage);

            lineImage->CreateImage(dbMemHDC, lineImagePath);
            lineImages.emplace_back(lineImage);
        }
        break;
	    }
    case WM_LBUTTONDOWN:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        RECT r;
        GetClientRect(hVideoTimeLine, &r);
        GetClientRect(hWnd, &r);
        dbWindow->OnPaint([&](const HDC hdc)
            {
                backgroundImage.OnPaint(hdc);
                for (auto lineImage : lineImages)
                    lineImage->OnPaint(hdc);
            });
        break;
    //case WM_NCHITTEST:
    //    break;
    case WM_SIZING: 
        return TRUE;
    case WM_CHAR:
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}
