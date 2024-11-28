#pragma once
#include "MediaFunction.h"
#include "WindowObject.h"

class BitmapController;

class VideoCaptureController : public WindowObject
{
private:
	static void BitmapsWorkerThread(VideoCaptureController* controller, double timeInterval);

public:
	VideoCaptureController();
	~VideoCaptureController() override;

	HRESULT OpenURL(const std::wstring wStrURL);
	void Release();
	void MakeVideoCaptures(double timeInterval);
	void OnPaint(HDC hdc) const;

	void SetBitmapsActive(bool value) const;
	void SetBitmapsRectToInterval(RECT& rectInterval);
	void SetBitmapsSize(POINT& size);

private:
	RECT interval;
	IMFSourceReader* m_pSourceReader;
	std::vector<BitmapController*> bitmaps;
	HANDLE makeMutex;  // ¹ÂÅØ½º ÇÚµé
};

