#include "VideoFrameReader.h"

VideoFrameReader::VideoFrameReader()
{
}

VideoFrameReader::VideoFrameReader(HWND _hwnd)
	: hwnd(_hwnd)
{
}

VideoFrameReader::~VideoFrameReader()
{
	bitmaps.clear();
	Release();
}

void VideoFrameReader::Release()
{
	makeSampleThread.request_stop();
	if (makeSampleThread.joinable())
		makeSampleThread.join();
	SafeRelease(&pReader);
}

void VideoFrameReader::OpenVideoAnsyc(PCWSTR url)
{
	Release();

	HRESULT hr;
	// IMFSourceReader의 속성을 통해 디코딩 활성화
	IMFAttributes* pAttributes = NULL;
	hr = MFCreateAttributes(&pAttributes, 1);
	if (SUCCEEDED(hr))
	{
		hr = pAttributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);
		if (SUCCEEDED(hr))
		{
			hr = MFCreateSourceReaderFromURL(url, pAttributes, &pReader);
		}
		SafeRelease(&pAttributes);
	}
	//hr = MFCreateSourceReaderFromURL(url, NULL, &pReader);
	if (FAILED(hr))
	{
		std::cerr << "Faild Open URL" << std::endl;
		return;
	}
	hr = ConfigureVideoDecoder(pReader);
	if (FAILED(hr))
	{
		std::cerr << "Faild Open URL" << std::endl;
		SafeRelease(&pReader);
		return;
	}
	
	int videoTime = std::ceil(ConvertNanoSecondsToSeconds(GetVideoTime(pReader)));
	
	std::lock_guard<std::mutex> lock(mtx);
	bitmaps.clear();
	bitmaps.resize(videoTime);
	bitmapLenth = 0;

	makeSampleThread = std::jthread([this](std::stop_token stop_token){MakeSample(stop_token); });
}

void VideoFrameReader::OnPain(HDC hdc)
{
	if (bitmaps.empty()) return;

	int len = GetBitmapLentgh();
	int c = GetVideoDuration();
	for (int i = 0; i < len; i++)
	{
		if (i == c)
		{
			int w = GetVideoDuration() * 100;
			w %= size.x;
			bitmaps[i].get()->OnPaint(hdc, w, size.y);
		}
		else
			bitmaps[i].get()->OnPaint(hdc);
	}
}

double VideoFrameReader::GetVideoDuration()
{
	return ConvertNanoSecondsToSeconds(GetVideoTime(pReader));
}

void VideoFrameReader::SetBitmapSize(int w, int h)
{
	size.x = w;
	size.y = h;
	int len = GetBitmapLentgh();
	for (int i = 0; i < len; i++)
	{
		bitmaps[i].get()->wTransform.SetSize(w, h);
	}
}

void VideoFrameReader::SetBitmapPositionInterval(int x, int y)
{
	positionInterval.x = x;
	positionInterval.y = y;
	int len = GetBitmapLentgh();
	for (int i = 0; i < len; i++)
	{
		bitmaps[i].get()->wTransform.SetPosition(x * i, y);
	}
}

void VideoFrameReader::MakeSample(std::stop_token stop_token)
{
	HRESULT hr;

	DWORD videoStreamIndex;
	DWORD pdwStreamFlags;
	LONGLONG pllTimestamp;
	IMFSample* pSample = NULL;
	hr = FindVideoStreamIndex(pReader, &videoStreamIndex);
	if (FAILED(hr)) return;

	for (int i = 0; i < bitmaps.size(); i++)
	{
		if (stop_token.stop_requested()) break;

		hr = SeekToTime(pReader, ConvertSecondsToNanoSeconds(i));
		if (FAILED(hr)) break;

		hr = pReader->ReadSample(
			MF_SOURCE_READER_FIRST_VIDEO_STREAM,
			0,
			&videoStreamIndex,
			&pdwStreamFlags,
			&pllTimestamp,
			&pSample);
		if (FAILED(hr)) break;

		UINT32 w, h;
		hr = GetVideoResolution(pReader, &w, &h);
		if (FAILED(hr)) break;

		IMFMediaBuffer* pBuffer = NULL;
		hr = pSample->GetBufferByIndex(0, &pBuffer);
		if (FAILED(hr)) break;

		BYTE* pData = NULL;
		DWORD dwDataLength = 0;
		hr = pBuffer->Lock(&pData, NULL, &dwDataLength);
		if (FAILED(hr))
		{
			SafeRelease(&pBuffer);
			break;
		}

		BYTE* pRGB = NV12ToRGB32(pData, w, h);
		bitmaps[i] = std::make_unique<BitmapController>();
		bitmaps[i].get()->wTransform.SetSize(size);
		bitmaps[i].get()->wTransform.SetPosition(positionInterval.x * i, positionInterval.y);
		bitmaps[i]->MakeBitmap(pRGB, w, h);
		SetChild(bitmaps[i].get());
		std::unique_lock<std::mutex> lock(mtx);
		bitmapLenth++;
		lock.unlock();

		InvalidateRect(hwnd, NULL, FALSE);  // 무효화 없이 갱신
		UpdateWindow(hwnd);

		pBuffer->Unlock();
		SafeRelease(&pBuffer);
	}

	SafeRelease(&pSample);
}

int VideoFrameReader::GetBitmapLentgh()
{
	std::lock_guard<std::mutex> lock(mtx);
	int len = bitmapLenth;

	return len;
}
