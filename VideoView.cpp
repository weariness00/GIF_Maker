#include "VideoView.h"

LRESULT VideoView::VideoViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    VideoView* pThis = NULL;

    if (uMsg == WM_NCCREATE) {
        // WM_NCCREATE���� lParam�� ���� this ������ ����
        LPCREATESTRUCT pCreate = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<VideoView*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else {
        // ���� �޽��������� GWLP_USERDATA���� this �����͸� ������
        pThis = reinterpret_cast<VideoView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis) {
        // ��� �Լ��� �޽��� ó��
        return pThis->VideoHandleMessage(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT VideoView::ImageWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    VideoView* pThis = NULL;

    if (uMsg == WM_NCCREATE) {
        // WM_NCCREATE���� lParam�� ���� this ������ ����
        LPCREATESTRUCT pCreate = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<VideoView*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else {
        // ���� �޽��������� GWLP_USERDATA���� this �����͸� ������
        pThis = reinterpret_cast<VideoView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis) {
        // ��� �Լ��� �޽��� ó��
        return pThis->ImageHandleMessage(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// --------------------------------------------------------------------------------
// dynamic Members
VideoView::VideoView(HWND hwnd): videoPlayer(nullptr)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    wTransform.SetRect({ 0,0,rect.right - rect.left , 400 });
    auto wRect = wTransform.GetWorldRect();

    // Video Window
    hVideo = CreateChildWindow(
        WS_EX_TOPMOST,
        L"VideoView",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_THICKFRAME | WS_TABSTOP,
        VideoViewWindowProc,
        *wRect,
        hwnd,
        GetModuleHandle(nullptr),
        this);
    OnCreateWindow(hVideo);

    // Video Overlay Layerd Image Window
    hImage = CreateChildWindow(
        WS_EX_LAYERED,
        L"Video Overlay Image View",
        NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE,
        ImageWindowProc,
        RECT(0,0,wRect->right, wRect->bottom),
        hVideo,
        GetModuleHandle(nullptr),
        this
    );
    SetLayeredWindowAttributes(hImage, 0, 255, LWA_ALPHA);
    // ETC...
    videoPlayer->readyVideoRendererEvent.AddEvent(std::function<void()>([&]()
        {
            videoPlayer->Pause();
        }));
}

VideoView::~VideoView()
{
    videoPlayer->Shutdown();
    SafeRelease(&videoPlayer);
}

LRESULT VideoView::VideoHandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_LBUTTONDOWN:
        SetFocus(hWnd); // Ŭ���� �ڽ� �����쿡 ��Ŀ���� �ش�.
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_NCHITTEST: {
        RECT rect;
        GetClientRect(hWnd, &rect);

        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ScreenToClient(hWnd, &pt);

        const int BORDER_SIZE = 2; // �׵θ� ũ��

        if (pt.x <= BORDER_SIZE) {
            if (pt.y <= BORDER_SIZE) return HTTOPLEFT; // ���� �� �𼭸�
            if (pt.y >= rect.bottom - BORDER_SIZE) return HTBOTTOMLEFT; // ���� �Ʒ� �𼭸�
            return HTLEFT; // ���� �׵θ�
        }
        if (pt.x >= rect.right - BORDER_SIZE) {
            if (pt.y <= BORDER_SIZE) return HTTOPRIGHT; // ������ �� �𼭸�
            if (pt.y >= rect.bottom - BORDER_SIZE) return HTBOTTOMRIGHT; // ������ �Ʒ� �𼭸�
            return HTRIGHT; // ������ �׵θ�
        }
        if (pt.y <= BORDER_SIZE) return HTTOP; // ��� �׵θ�
        if (pt.y >= rect.bottom - BORDER_SIZE) return HTBOTTOM; // �ϴ� �׵θ�
        break;
    }
    case WM_SIZING:{
        // Ŭ���̾�Ʈ ������ ũ�� ���
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);  // Ŭ���̾�Ʈ ������ ũ��

        OnResize(clientRect);
        return TRUE;
    }
    case WM_PAINT:{
        break;
    }
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

LRESULT VideoView::ImageHandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CREATE: {
        dbWindow = new DoubleBufferingWindow(hWnd);
        auto dbMemHDC = dbWindow->GetMemHDC();
        GDIPlusManager::Instance->CreateGraphics(dbMemHDC);
        break;
    }
    case WM_PAINT: {
        dbWindow->OnPaint([&](const HDC hdc)
        {
            gifAreaImage.OnPaint(hdc);
        });
        break;
    }
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

void VideoView::OnFileOpen(std::wstring& path)
{
    videoFilePath = path;
    videoPlayer->OpenURL(path.c_str());
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

void VideoView::OnPaint(HDC hdc)
{
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
