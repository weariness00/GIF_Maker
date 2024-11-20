#pragma once
#include "framework.h"
#include "VideoPlayer.h"

class VideoView
{
public:
	VideoView(HWND hwnd);
	~VideoView();

public:
	void OnFileOpen(std::wstring& path) const;
	LRESULT OnCreateWindow(HWND hwnd);
	void OnPaint(HWND hwnd);
	void OnResize(WORD width, WORD height);
	void OnKeyPress(WPARAM key);
	void OnPlayerEvent(HWND hwnd, WPARAM pUnkPtr);
	void UpdateUI(VideoPlayerState state);
	void NotifyError(HWND hwnd, PCWSTR pszErrorMessage, HRESULT hrErr);
private:
	HWND hVideo;
	BOOL repaintClient = TRUE;
	VideoPlayer* videoPlayer;
};

