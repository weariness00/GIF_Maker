#include "VideoView.h"

VideoView::VideoView(HWND hwnd)
{
    // Initialize the player object.
    HRESULT hr = VideoPlayer::CreateInstance(hwnd, hwnd, &videoPlayer);
    if (SUCCEEDED(hr))
    {
        UpdateUI(Closed);
    }
    else
    {
        NotifyError(NULL, L"Could not initialize the player object.", hr);
    }
}

VideoView::~VideoView()
{
    videoPlayer->Shutdown();
    SafeRelease(&videoPlayer);
}

void VideoView::OnFileOpen(std::wstring& path) const
{
    videoPlayer->OpenURL(path.c_str());
//
//    IFileOpenDialog* pFileOpen = NULL;
//    IShellItem* pItem = NULL;
//    PWSTR pszFilePath = NULL;
//
//    // Create the FileOpenDialog object.
//    HRESULT hr = CoCreateInstance(__uuidof(FileOpenDialog), NULL,
//        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileOpen));
//    if (FAILED(hr))
//    {
//        goto done;
//    }
//
//    // Show the Open dialog box.
//    hr = pFileOpen->Show(NULL);
//    if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
//    {
//        // The user canceled the dialog. Do not treat as an error.
//        hr = S_OK;
//        goto done;
//    }
//    else if (FAILED(hr))
//    {
//        goto done;
//    }
//
//    // Get the file name from the dialog box.
//    hr = pFileOpen->GetResult(&pItem);
//    if (FAILED(hr))
//    {
//        goto done;
//    }
//
//    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
//    if (FAILED(hr))
//    {
//        goto done;
//    }
//
//    // Display the file name to the user.
//    hr = videoPlayer->OpenURL(pszFilePath);
//    if (SUCCEEDED(hr))
//    {
//        UpdateUI(hwnd, OpenPending);
//    }
//
//done:
//    if (FAILED(hr))
//    {
//        NotifyError(hwnd, L"Could not open the file.", hr);
//        UpdateUI(hwnd, Closed);
//    }
//    CoTaskMemFree(pszFilePath);
//    SafeRelease(&pItem);
//    SafeRelease(&pFileOpen);
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
        NotifyError(NULL, L"Could not initialize the player object.", hr);
        return -1;  // Destroy the window
    }
}

void VideoView::OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    if (videoPlayer && videoPlayer->HasVideo())
    {
        // Video is playing. Ask the player to repaint.
        videoPlayer->Repaint();
    }
    else
    {
        // The video is not playing, so we must paint the application window.
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, (HBRUSH)COLOR_WINDOW);
    }
    EndPaint(hwnd, &ps);
}

void VideoView::OnResize(WORD width, WORD height)
{
    if (videoPlayer)
    {
        videoPlayer->ResizeVideo(width, height);
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

void VideoView::OnPlayerEvent(HWND hwnd, WPARAM pUnkPtr)
{
    HRESULT hr = videoPlayer->HandleEvent(pUnkPtr);
    if (FAILED(hr))
    {
        NotifyError(hwnd, L"An error occurred.", hr);
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

void VideoView::NotifyError(HWND hwnd, PCWSTR pszErrorMessage, HRESULT hrErr)
{
    const size_t MESSAGE_LEN = 512;
    WCHAR message[MESSAGE_LEN];

    if (SUCCEEDED(StringCchPrintf(message, MESSAGE_LEN, L"%s (HRESULT = 0x%X)",
        pszErrorMessage, hrErr)))
    {
        MessageBox(hwnd, message, NULL, MB_OK | MB_ICONERROR);
    }
}
