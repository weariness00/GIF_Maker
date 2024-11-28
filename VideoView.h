#pragma once
#include "framework.h"
#include "VideoTimeLineView.h"

class VideoView : public WindowObject
{
private:
	static LRESULT CALLBACK VideoViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	VideoView(HWND hwnd);
	~VideoView();

public:
	void OnFileOpen(std::wstring& path);
	LRESULT OnCreateWindow(HWND hwnd);
	void OnPaint();
	void OnResize(RECT& rect) const;
	void OnResize(int w, int h) { RECT rect{ 0,0, w, h }; OnResize(rect); }
	void OnKeyPress(WPARAM key);
	void OnPlayerEvent(WPARAM pUnkPtr);
	void UpdateUI(VideoPlayerState state);
	void NotifyError(PCWSTR pszErrorMessage, HRESULT hrErr);

	std::wstring* GetVideoPath() { return &videoFilePath; }
protected:
	LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	HWND hVideo;

private:
	BOOL repaintClient = TRUE;
	VideoPlayer* videoPlayer;

	std::wstring videoFilePath;
};

