#include "VideoView.h"

LRESULT VideoView::VideoViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    VideoView* pThis = NULL;

    if (uMsg == WM_NCCREATE) {
        // WM_NCCREATE에서 lParam을 통해 this 포인터 설정
        LPCREATESTRUCT pCreate = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<VideoView*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else {
        // 이후 메시지에서는 GWLP_USERDATA에서 this 포인터를 가져옴
        pThis = reinterpret_cast<VideoView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis) {
        // 멤버 함수로 메시지 처리
        return pThis->HandleMessage(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

VideoView::VideoView(HWND hwnd): videoPlayer(nullptr)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    x = y = 0;
    windowW = rect.right - rect.left;
	windowH = 400;
    hVideo = CreateChildWindow(
        hwnd,
        GetModuleHandle(nullptr),
        L"VideoView",
        VideoViewWindowProc,
        x, y,
        windowW, windowH,
        this);
    OnCreateWindow(hVideo);

    videoTimeLineView = new VideoTimeLineView(hwnd);
    videoTimeLineView->OnResize(0,400, windowW, 400);
}

VideoView::~VideoView()
{
    videoPlayer->Shutdown();
    SafeRelease(&videoPlayer);
}

LRESULT VideoView::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_LBUTTONDOWN:
        SetFocus(hVideo); // 클릭한 자식 윈도우에 포커스를 준다.
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_NCHITTEST: {
        RECT rect;
        GetClientRect(hWnd, &rect);

        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ScreenToClient(hWnd, &pt);

        const int BORDER_SIZE = 2; // 테두리 크기

        if (pt.x <= BORDER_SIZE) {
            if (pt.y <= BORDER_SIZE) return HTTOPLEFT; // 왼쪽 위 모서리
            if (pt.y >= rect.bottom - BORDER_SIZE) return HTBOTTOMLEFT; // 왼쪽 아래 모서리
            return HTLEFT; // 왼쪽 테두리
        }
        if (pt.x >= rect.right - BORDER_SIZE) {
            if (pt.y <= BORDER_SIZE) return HTTOPRIGHT; // 오른쪽 위 모서리
            if (pt.y >= rect.bottom - BORDER_SIZE) return HTBOTTOMRIGHT; // 오른쪽 아래 모서리
            return HTRIGHT; // 오른쪽 테두리
        }
        if (pt.y <= BORDER_SIZE) return HTTOP; // 상단 테두리
        if (pt.y >= rect.bottom - BORDER_SIZE) return HTBOTTOM; // 하단 테두리
        break;
    }
    case WM_SIZING:{
        // 클라이언트 영역의 크기 얻기
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);  // 클라이언트 영역의 크기

        OnResize(clientRect);
        return TRUE;
    }
    //case WM_PAINT:
    //    OnPaint();
    //    break;
    //case WM_SIZE:
    //    OnResize(windowH, windowH);
    //    break;
    case WM_CHAR:
        OnKeyPress(wParam);
        break;
    case WM_APP_PLAYER_EVENT:
        OnPlayerEvent(wParam);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

void VideoView::OnFileOpen(std::wstring& path) const
{
    videoPlayer->OpenURL(path.c_str());
    //OnResize(windowW, windowH);
}

LRESULT VideoView::OnCreateWindow(HWND hwnd)
{
    // Initialize the player object.
    HRESULT hr = VideoPlayer::CreateInstance(hwnd, hwnd, &videoPlayer);
    if (SUCCEEDED(hr))
    {
        UpdateUI(Closed);
        return 0;   // Success.
    }
    else
    {
        NotifyError(L"Could not initialize the player object.", hr);
        return -1;  // Destroy the window
    }
}

void VideoView::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hVideo, &ps);

    if (videoPlayer && videoPlayer->HasVideo())
    {
        // Video is playing. Ask the player to repaint.
        videoPlayer->Repaint();
    }
    else
    {
        // The video is not playing, so we must paint the application window.
        RECT rc;
        GetClientRect(hVideo, &rc);
        FillRect(hdc, &rc, (HBRUSH)COLOR_WINDOW);
    }
    EndPaint(hVideo, &ps);
}

void VideoView::OnResize(RECT& rect) const
{
    if (videoPlayer)
    {
        videoPlayer->ResizeVideo(rect);
    }
}

void VideoView::OnKeyPress(WPARAM key)
{
    switch (key)
    {
        // Space key toggles between running and paused
    case VK_SPACE:
        if (videoPlayer->GetState() == Started)
        {
            videoPlayer->Pause();
        }
        else if (videoPlayer->GetState() == Paused)
        {
            videoPlayer->Play();
        }
        break;
    }
}

void VideoView::OnPlayerEvent(WPARAM pUnkPtr)
{
    HRESULT hr = videoPlayer->HandleEvent(pUnkPtr);
    if (FAILED(hr))
    {
        NotifyError(L"An error occurred.", hr);
    }
    UpdateUI(videoPlayer->GetState());
}

void VideoView::UpdateUI(VideoPlayerState state)
{
    BOOL bWaiting = FALSE;
    BOOL bPlayback = FALSE;

    assert(videoPlayer != NULL);

    switch (state)
    {
    case OpenPending:
        bWaiting = TRUE;
        break;

    case Started:
        bPlayback = TRUE;
        break;

    case Paused:
        bPlayback = TRUE;
        break;
    }

    //HMENU hMenu = GetMenu(hwnd);
    //UINT  uEnable = MF_BYCOMMAND | (bWaiting ? MF_GRAYED : MF_ENABLED);

    if (bPlayback && videoPlayer->HasVideo())
    {
        repaintClient = FALSE;
    }
    else
    {
        repaintClient = TRUE;
    }
}

void VideoView::NotifyError(PCWSTR pszErrorMessage, HRESULT hrErr)
{
    const size_t MESSAGE_LEN = 512;
    WCHAR message[MESSAGE_LEN];

    if (SUCCEEDED(StringCchPrintf(message, MESSAGE_LEN, L"%s (HRESULT = 0x%X)",
        pszErrorMessage, hrErr)))
    {
        MessageBox(hVideo, message, NULL, MB_OK | MB_ICONERROR);
    }
}
