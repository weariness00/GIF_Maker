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


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
// Convert Format Data
BYTE* NV12ToRGB32(const BYTE* pNV12, int width, int height);

//IMFSourceReader Associate Function
HRESULT SeekToTime(IMFSourceReader* pReader, LONGLONG timeInHundredNanoSeconds);
LONGLONG GetVideoCurrentTime(IMFSourceReader* pReader);
LONGLONG GetVideoTime(IMFSourceReader* pReader);
HRESULT GetVideoResolution(IMFSourceReader* pReader, UINT32* pWidth, UINT32* pHeight);
GUID GetVideoFormat(IMFSourceReader* pMediaSource);
double ConvertNanoSecondsToSeconds(LONGLONG time100ns);
LONGLONG ConvertSecondsToNanoSeconds(double seconds);

DWORD GetStreamLength(IMFSourceReader* pReader);
HRESULT FindVideoStreamIndex(IMFSourceReader* pReader, DWORD* pdwVideoStreamIndex);
HRESULT ConfigureVideoDecoder(IMFSourceReader* pReader);
HRESULT CreateSourceReaderAsync(
    PCWSTR pszURL,
    IMFSourceReaderCallback* pCallback,
    IMFSourceReader** ppReader);