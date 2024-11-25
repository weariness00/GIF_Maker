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
    wTransform.SetSize(rect.right, rect.bottom);
    hVideoTimeLine = CreateChildWindow(
        hwnd,
        GetModuleHandle(nullptr),
        L"VideoTimeLineView",
        WindowProc,
        rect,
        this,
        RGB(200,200,200));

    pixelPerSecond = 100;
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

std::pair<float, float> VideoTimeLineView::GetTime() const
{
    auto x1 = timeBarImages[0]->wTransform.GetLocalPosition().x;
    auto x2 = timeBarImages[1]->wTransform.GetLocalPosition().x;

    auto minX = min(x1, x2);
    auto maxX = max(x1, x2);
    float startTime = minX / pixelPerSecond;
    float endTime = maxX / pixelPerSecond;

    return { startTime , endTime };
}

LRESULT VideoTimeLineView::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CREATE:{
			dbWindow = new DoubleBufferingWindow(hWnd);
			auto dbMemHDC = dbWindow->GetMemHDC();
			backgroundImage.wTransform.SetRect(0,0, wTransform.GetSize());
			backgroundImage.CreateImage(dbMemHDC, backgroundImagePath);
			SetChild(&backgroundImage);

            // Time Image Associate Objects
            timeAssociateObject.wTransform.SetPosition(100, 0);
			SetChild(&timeAssociateObject);

            // Time Line
            lineImages = new LineImages(dbMemHDC, 30);
            timeAssociateObject.SetChild(lineImages);

            // Time Start & End Line
            for (int i = 0; i < 2; i++)
            {
                timeBarImages[i] = new TimeBarImage(dbMemHDC);
                timeBarImages[i]->wTransform.SetPosition(i * 100, 0);
                timeAssociateObject.SetChild(timeBarImages[i]);
            }
			break;
	    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        dbWindow->OnPaint([&](const HDC hdc)
            {
                backgroundImage.OnPaint(hdc);
                lineImages->OnPaint(hdc);
                for (auto& timeBarImage : timeBarImages)
                    timeBarImage->OnPaint(hdc);
            });
        break;
    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE:
    case WM_LBUTTONUP:
        for (int i = 0; i < 2; i++)
        {
            timeBarImages[i]->OnMouseEvent(uMsg, dbWindow->GetMemHDC(), timeBarImages[i], lParam);
        }
        break;
    case WM_MOUSEWHEEL:

        break;
    case WM_SIZING: 
        return TRUE;
    case WM_CHAR:
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}


//---------------------------------------------------------------------
// Line Image
LineImages::LineImages(HDC hdc, int imageLength)
{
    name = "Images Parent";

    for (int i = 0; i < imageLength; i++)
    {
        ImageController* lineImage = new ImageController();
        lineImage->name = "Line Image";
        lineImage->wTransform.SetRect({ i * 60, 0, 5, 2000 });
        lineImage->CreateImage(hdc, lineImagePath);
        SetChild(lineImage);

        lineImages.emplace_back(lineImage);
    }
}

LineImages::~LineImages()
{
}

void LineImages::OnPaint(HDC hdc)
{
    for (auto lineImage : lineImages)
        lineImage->OnPaint(hdc);
}

//---------------------------------------------------------------------
// 
TimeBarImage::TimeBarImage(HDC hdc)
{
    name = "Time Bar Images Parent";

    LONG defaultSize = 12;

    wTransform.SetSize(defaultSize, 3000);

    timeBarImage.CreateImage(hdc, timeBarImagePath);
    timeBarImage.wTransform.SetPosition(-defaultSize / 2, 0);
    timeBarImage.wTransform.SetSize(defaultSize, defaultSize);
    SetChild(&timeBarImage);

    timeBarLineImage.CreateImage(hdc, timeBarLineImagePath);
    timeBarLineImage.wTransform.SetPosition(-defaultSize / 4, 0);
    timeBarLineImage.wTransform.SetSize(defaultSize / 2, 3000);
    SetChild(&timeBarLineImage);
}

TimeBarImage::~TimeBarImage()
{
}

void TimeBarImage::OnPaint(HDC hdc)
{
    timeBarLineImage.OnPaint(hdc);
    timeBarImage.OnPaint(hdc);
}

void TimeBarImage::MousePressEvent(const MouseEvent& mouseEvent)
{
	WindowMouseEventInterface::MousePressEvent(mouseEvent);
    POINT worldPos = wTransform.GetWorldPosition();
    POINT localPos = wTransform.GetLocalPosition();
    wTransform.SetPosition(mouseEvent.x - (worldPos.x - localPos.x), 0);
}
