#include "VideoCaptureController.h"
#include "BitmapController.h"
#include "MediaFunction.h"
#include "GDIPlusManager.h"
#include <thread>

void VideoCaptureController::BitmapsWorkerThread(VideoCaptureController* controller, double timeInterval)
{
    double videoDuration = ConvertNanoSecondsToSeconds(GetVideoTime(controller->m_pSourceReader));
    int frameCount = videoDuration / timeInterval;
    auto llTimeInterval = ConvertSecondsToNanoSeconds(timeInterval);
    LONGLONG frameTime = 0;
    UINT w, h;
    int i = 0;

    auto videoFormat = GetVideoFormat(controller->m_pSourceReader);
    while (i != frameCount)
    {
        DWORD dwWaitResult = WaitForSingleObject(controller->makeMutex, INFINITE);
        if (dwWaitResult == WAIT_FAILED ||
            controller->m_pSourceReader == nullptr)
            break;
        if (dwWaitResult != WAIT_OBJECT_0) continue;

        auto frame = ReadFrameAtTime(controller->m_pSourceReader, frameTime, w, h);
        auto bitmap = MakeBitmapToFrame(frame, w, h, videoFormat);
        BitmapController* bitmapController = new BitmapController;
        bitmapController->SetBitmap(bitmap);
        bitmapController->wTransform.SetRect(i * controller->interval.left, 0, w, h);
        //bitmapController->wTransform.SetRect(i * controller->interval.left, 0, controller->interval.right, controller->interval.bottom);
        controller->bitmaps.emplace_back(bitmapController);

        delete frame;
        frameTime += llTimeInterval;
        i++;

        // �۾� �� ���ؽ��� ����
        ReleaseMutex(controller->makeMutex);
    }
}

VideoCaptureController::VideoCaptureController()
{
}

VideoCaptureController::~VideoCaptureController()
{
    Release();
}

HRESULT VideoCaptureController::OpenURL(const std::wstring wStrURL)
{
    Release();
    HRESULT hr;
    makeMutex = CreateMutex(
        NULL,    // �⺻ ���� �Ӽ�
        FALSE,   // �����ڰ� ���� (�ʱ� ���´� ���ؽ� ȹ�� �ȵ�)
        NULL     // �̸� ����
    );

    // �̵�� ���� ����
    IMFAttributes* pAttributes = nullptr;
    hr = MFCreateSourceReaderFromURL(wStrURL.c_str(), pAttributes, &m_pSourceReader);
	if (FAILED(hr)) {
        return hr;
    }

    // ���� ��Ʈ�� ����
    hr = m_pSourceReader->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE);
	return hr;
}

void VideoCaptureController::Release()
{
    if (makeMutex) {
        DWORD dwWaitResult = WaitForSingleObject(makeMutex, INFINITE);
        for (auto bitmap : bitmaps)
            delete bitmap;
        bitmaps.clear();

        if (m_pSourceReader)
        {
            m_pSourceReader->Release();
            m_pSourceReader = nullptr;
        }

        ReleaseMutex(makeMutex);
        CloseHandle(makeMutex);
        makeMutex = nullptr;  // ���� �� �ڵ� �η� �ʱ�ȭ
    }
}

void VideoCaptureController::MakeVideoCaptures(double timeInterval)
{
	std::thread worker(&VideoCaptureController::BitmapsWorkerThread, this, timeInterval);

	worker.detach();
}

void VideoCaptureController::OnPaint(HDC hdc) const
{
    int count = bitmaps.size(); // �����忡�� �����ϰ� �������� �̸� ������ ī��Ʈ ��������
    for (int i = 0; i < bitmaps.size(); i++)
    {
        bitmaps[i]->OnPaint(hdc);
    }
}

void VideoCaptureController::SetBitmapsActive(bool value) const
{
    int count = bitmaps.size();
    for (int i = 0; i < count; i++)
    {
        bitmaps[i]->SetActive(value);
    }
}

void VideoCaptureController::SetBitmapsRectToInterval(RECT& rectInterval)
{
    interval = rectInterval;
    int x = rectInterval.left;
    int count = bitmaps.size();
    for (int i = 0; i < count; i++)
    {
        auto size = bitmaps[i]->wTransform.GetSize();
        bitmaps[i]->wTransform.SetRect(i * x, 0, size.x, size.y);
    }
}

void VideoCaptureController::SetBitmapsSize(POINT& size)
{
    interval.right = size.x;
    interval.bottom = size.y;
    int count = bitmaps.size();
    for (int i = 0; i < count; i++)
    {
        bitmaps[i]->wTransform.SetSize(size);
    }
}