#pragma once
#include <windows.h>
#include <shobjidl.h> 
#include <shlwapi.h>
#include <assert.h>
#include <strsafe.h>

// Media Foundation headers
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "evr.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "gdiplus.lib")
#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <propvarutil.h>
#include <evr.h>
#include <gdiplus.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

HRESULT CreateMediaSource(PCWSTR sURL, IMFMediaSource** ppSource);
HRESULT CreateMediaSinkActivate(
    IMFStreamDescriptor* pSourceSD,     // Pointer to the stream descriptor.
    HWND hVideoWindow,                  // Handle to the video clipping window.
    IMFActivate** ppActivate
);

HRESULT AddSourceNode(
    IMFTopology* pTopology,           // Topology.
    IMFMediaSource* pSource,          // Media source.
    IMFPresentationDescriptor* pPD,   // Presentation descriptor.
    IMFStreamDescriptor* pSD,         // Stream descriptor.
    IMFTopologyNode** ppNode);         // Receives the node pointer.

// Add an output node to a topology.
HRESULT AddOutputNode(
    IMFTopology* pTopology,     // Topology.
    IMFActivate* pActivate,     // Media sink activation object.
    DWORD dwId,                 // Identifier of the stream sink.
    IMFTopologyNode** ppNode);   // Receives the node pointer.

HRESULT AddBranchToPartialTopology(
    IMFTopology* pTopology,         // Topology.
    IMFMediaSource* pSource,        // Media source.
    IMFPresentationDescriptor* pPD, // Presentation descriptor.
    DWORD iStream,                  // Stream index.
    HWND hVideoWnd);                 // Window for video playback.

//  Create a playback topology from a media source.
HRESULT CreatePlaybackTopology(
    IMFMediaSource* pSource,          // Media source.
    IMFPresentationDescriptor* pPD,   // Presentation descriptor.
    HWND hVideoWnd,                   // Video window.
    IMFTopology** ppTopology);         // Receives a pointer to the topology.

// Get Frame & Bitmap To IMFSourceReader
BYTE* ReadFrame(IMFSourceReader* pReader, UINT32& width, UINT32& height);
BYTE* ReadFrameAtTime(IMFSourceReader* pReader, LONGLONG timeInHundredNanoSeconds, UINT32& width, UINT32& height);
Gdiplus::Bitmap* MakeBitmapToFrame(BYTE* frameByte, const UINT& width, const UINT& height, const GUID& videoFormat);

//IMFSourceReader Associate Function
HRESULT SeekToTime(IMFSourceReader* pReader, LONGLONG timeInHundredNanoSeconds);
LONGLONG GetVideoCurrentTime(IMFSourceReader* pReader);
LONGLONG GetVideoTime(IMFSourceReader* pReader);
GUID GetVideoFormat(IMFSourceReader* pMediaSource);
double ConvertNanoSecondsToSeconds(LONGLONG time100ns);
LONGLONG ConvertSecondsToNanoSeconds(double seconds);

CLSID* GetEncoderClsid(const WCHAR* format);

//Video Format Associate Function
HRESULT InitializeDecoder(IMFTransform** ppDecoder, const GUID& inputFormat);
HRESULT ConfigureDecoder(IMFTransform* pDecoder, const GUID& inputFormat, UINT32 width, UINT32 height);
HRESULT ProcessSample(IMFTransform* pDecoder, IMFSample* pInputSample, IMFSample** ppOutputSample);
