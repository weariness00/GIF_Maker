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
	pixelPerSecond = 100;
	wTransform.SetSize(rect.right, rect.bottom);
	hVideoTimeLine = CreateChildWindow(
		hwnd,
		GetModuleHandle(nullptr),
		L"VideoTimeLineView",
		WindowProc,
		rect,
		this,
		RGB(200, 200, 200));
}

VideoTimeLineView::~VideoTimeLineView()
{
	auto dbMemHDC = dbWindow->GetMemHDC();
	GDIPlusManager::Instance->ReleaseGraphics(dbMemHDC);

	delete dbWindow;
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
	case WM_CREATE: {
		dbWindow = new DoubleBufferingWindow(hWnd);
		auto dbMemHDC = dbWindow->GetMemHDC();
		GDIPlusManager::Instance->CreateGraphics(dbMemHDC);

		backgroundImage.wTransform.SetRect(0, 0, wTransform.GetSize());
		backgroundImage.CreateImage(backgroundImagePath);
		SetChild(&backgroundImage);

		// Time Image Associate Objects
		timeAssociateObject.wTransform.SetPosition(100, 0);
		SetChild(&timeAssociateObject);

		// Time Line
		timeLineObjects = new TimeLineObjects(dbMemHDC, 30);
		timeLineObjects->SetPixelPerSeconds(pixelPerSecond);
		timeAssociateObject.SetChild(timeLineObjects);

		// Time Start & End Line
		for (int i = 0; i < 2; i++)
		{
			timeBarImages[i] = new TimeBarImage(dbMemHDC);
			timeBarImages[i]->wTransform.SetPosition(i * pixelPerSecond, 0);
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
				timeLineObjects->OnPaint(hdc);
				for (auto& timeBarImage : timeBarImages)
					timeBarImage->OnPaint(hdc);

				if(videoCapture) videoCapture->OnPaint(hdc);
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
// Time Line Image
// Time Line Text
std::wstring TimeLineObjects::TimeFormat(const float seconds)
{
	int totalSeconds = static_cast<int>(seconds);  // 정수 초
	int hours = totalSeconds / 3600;              // 시 계산
	int minutes = (totalSeconds % 3600) / 60;     // 분 계산
	int secs = totalSeconds % 60;                 // 초 계산
	int milliseconds = static_cast<int>((seconds - totalSeconds) * 1000); // 밀리초 계산

	std::wostringstream oss;
	if (hours > 0)
		oss << std::setw(2) << std::setfill(L'0') << hours << L":";
	oss << std::setw(2) << std::setfill(L'0') << minutes << L":"
		<< std::setw(2) << std::setfill(L'0') << secs;
	//<< std::setw(3) << std::setfill(L'0') << milliseconds;
	return oss.str();
}

TimeLineObjects::TimeLineObjects(HDC hdc, int objectLength)
{
	name = "Time Line Objects Parent";

	timeTextBoxSizeX = 30;
	for (int i = 0; i < objectLength; i++)
	{
		TextController* timeText = new TextController();
		timeText->name = "Time Text";
		SetChild(timeText);

		timeTexts.emplace_back(timeText);
	}

	for (int i = 0; i < objectLength; i++)
	{
		ImageController* lineImage = new ImageController();
		lineImage->name = "Line Image";
		lineImage->CreateImage(lineImagePath);
		SetChild(lineImage);

		lineImages.emplace_back(lineImage);
	}
}

TimeLineObjects::~TimeLineObjects()
{
}

void TimeLineObjects::OnPaint(HDC hdc)
{
	for (auto lineImage : lineImages)
		lineImage->OnPaint(hdc);

	for (auto timeText : timeTexts)
		timeText->OnPaint(hdc);
}

void TimeLineObjects::SetPixelPerSeconds(const int pixelPerSeconds)
{
	for (int i = 0; i < timeTexts.size(); i++)
	{
		timeTexts[i]->wTransform.SetRect(i * pixelPerSeconds - timeTextBoxSizeX / 2, 0, timeTextBoxSizeX, 10);
	}

	for (int i = 0; i < lineImages.size(); i++)
	{
		lineImages[i]->wTransform.SetRect(i * pixelPerSeconds - 6 / 2, 0, 6, 2000);
	}

	for (auto timeText : timeTexts)
	{
		auto pixelPosX = timeText->wTransform.GetLocalPosition().x + timeTextBoxSizeX / 2;
		timeText->text = TimeLineObjects::TimeFormat(float(pixelPosX) / pixelPerSeconds);
	}
}

//---------------------------------------------------------------------
// 
TimeBarImage::TimeBarImage(HDC hdc)
{
	name = "Time Bar Images Parent";

	LONG defaultSize = 12;

	wTransform.SetSize(defaultSize, 3000);

	timeBarImage.CreateImage(timeBarImagePath);
	timeBarImage.wTransform.SetPosition(-defaultSize / 2, 20);
	timeBarImage.wTransform.SetSize(defaultSize, defaultSize);
	SetChild(&timeBarImage);

	timeBarLineImage.CreateImage(timeBarLineImagePath);
	timeBarLineImage.wTransform.SetPosition(-defaultSize / 4, 20);
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
