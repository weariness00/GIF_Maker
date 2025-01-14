#include "MediaFunction.h"
#include <iostream>

//  Create a media source from a URL.
HRESULT CreateMediaSource(PCWSTR sURL, IMFMediaSource** ppSource)
{
    MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;

    IMFSourceResolver* pSourceResolver = NULL;
    IUnknown* pSource = NULL;

    // Create the source resolver.
    HRESULT hr = MFCreateSourceResolver(&pSourceResolver);
    if (FAILED(hr))
    {
        goto done;
    }

    // Use the source resolver to create the media source.

    // Note: For simplicity this sample uses the synchronous method to create 
    // the media source. However, creating a media source can take a noticeable
    // amount of time, especially for a network source. For a more responsive 
    // UI, use the asynchronous BeginCreateObjectFromURL method.

    hr = pSourceResolver->CreateObjectFromURL(
        sURL,                       // URL of the source.
        MF_RESOLUTION_MEDIASOURCE,  // Create a source object.
        NULL,                       // Optional property store.
        &ObjectType,        // Receives the created object type. 
        &pSource            // Receives a pointer to the media source.
    );
    if (FAILED(hr))
    {
        goto done;
    }

    // Get the IMFMediaSource interface from the media source.
    hr = pSource->QueryInterface(IID_PPV_ARGS(ppSource));

done:
    SafeRelease(&pSourceResolver);
    SafeRelease(&pSource);
    return hr;
}

//  Create an activation object for a renderer, based on the stream media type.

HRESULT CreateMediaSinkActivate(
    IMFStreamDescriptor* pSourceSD,     // Pointer to the stream descriptor.
    HWND hVideoWindow,                  // Handle to the video clipping window.
    IMFActivate** ppActivate
)
{
    IMFMediaTypeHandler* pHandler = NULL;
    IMFActivate* pActivate = NULL;

    // Get the media type handler for the stream.
    HRESULT hr = pSourceSD->GetMediaTypeHandler(&pHandler);
    if (FAILED(hr))
    {
        goto done;
    }

    // Get the major media type.
    GUID guidMajorType;
    hr = pHandler->GetMajorType(&guidMajorType);
    if (FAILED(hr))
    {
        goto done;
    }

    // Create an IMFActivate object for the renderer, based on the media type.
    if (MFMediaType_Audio == guidMajorType)
    {
        // Create the audio renderer.
        hr = MFCreateAudioRendererActivate(&pActivate);
    }
    else if (MFMediaType_Video == guidMajorType)
    {
        // Create the video renderer.
        hr = MFCreateVideoRendererActivate(hVideoWindow, &pActivate);
    }
    else
    {
        // Unknown stream type. 
        hr = E_FAIL;
        // Optionally, you could deselect this stream instead of failing.
    }
    if (FAILED(hr))
    {
        goto done;
    }

    // Return IMFActivate pointer to caller.
    *ppActivate = pActivate;
    (*ppActivate)->AddRef();

done:
    SafeRelease(&pHandler);
    SafeRelease(&pActivate);
    return hr;
}

// Add a source node to a topology.
HRESULT AddSourceNode(
    IMFTopology* pTopology,           // Topology.
    IMFMediaSource* pSource,          // Media source.
    IMFPresentationDescriptor* pPD,   // Presentation descriptor.
    IMFStreamDescriptor* pSD,         // Stream descriptor.
    IMFTopologyNode** ppNode)         // Receives the node pointer.
{
    IMFTopologyNode* pNode = NULL;

    // Create the node.
    HRESULT hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pNode);
    if (FAILED(hr))
    {
        goto done;
    }

    // Set the attributes.
    hr = pNode->SetUnknown(MF_TOPONODE_SOURCE, pSource);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPD);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSD);
    if (FAILED(hr))
    {
        goto done;
    }

    // Add the node to the topology.
    hr = pTopology->AddNode(pNode);
    if (FAILED(hr))
    {
        goto done;
    }

    // Return the pointer to the caller.
    *ppNode = pNode;
    (*ppNode)->AddRef();

done:
    SafeRelease(&pNode);
    return hr;
}

// Add an output node to a topology.
HRESULT AddOutputNode(
    IMFTopology* pTopology,     // Topology.
    IMFActivate* pActivate,     // Media sink activation object.
    DWORD dwId,                 // Identifier of the stream sink.
    IMFTopologyNode** ppNode)   // Receives the node pointer.
{
    IMFTopologyNode* pNode = NULL;

    // Create the node.
    HRESULT hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pNode);
    if (FAILED(hr))
    {
        goto done;
    }

    // Set the object pointer.
    hr = pNode->SetObject(pActivate);
    if (FAILED(hr))
    {
        goto done;
    }

    // Set the stream sink ID attribute.
    hr = pNode->SetUINT32(MF_TOPONODE_STREAMID, dwId);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);
    if (FAILED(hr))
    {
        goto done;
    }

    // Add the node to the topology.
    hr = pTopology->AddNode(pNode);
    if (FAILED(hr))
    {
        goto done;
    }

    // Return the pointer to the caller.
    *ppNode = pNode;
    (*ppNode)->AddRef();

done:
    SafeRelease(&pNode);
    return hr;
}
//</SnippetPlayer.cpp>

//  Add a topology branch for one stream.
//
//  For each stream, this function does the following:
//
//    1. Creates a source node associated with the stream. 
//    2. Creates an output node for the renderer. 
//    3. Connects the two nodes.
//
//  The media session will add any decoders that are needed.

HRESULT AddBranchToPartialTopology(
    IMFTopology* pTopology,         // Topology.
    IMFMediaSource* pSource,        // Media source.
    IMFPresentationDescriptor* pPD, // Presentation descriptor.
    DWORD iStream,                  // Stream index.
    HWND hVideoWnd)                 // Window for video playback.
{
    IMFStreamDescriptor* pSD = NULL;
    IMFActivate* pSinkActivate = NULL;
    IMFTopologyNode* pSourceNode = NULL;
    IMFTopologyNode* pOutputNode = NULL;

    BOOL fSelected = FALSE;

    HRESULT hr = pPD->GetStreamDescriptorByIndex(iStream, &fSelected, &pSD);
    if (FAILED(hr))
    {
        goto done;
    }

    if (fSelected)
    {
        // Create the media sink activation object.
        hr = CreateMediaSinkActivate(pSD, hVideoWnd, &pSinkActivate);
        if (FAILED(hr))
        {
            goto done;
        }

        // Add a source node for this stream.
        hr = AddSourceNode(pTopology, pSource, pPD, pSD, &pSourceNode);
        if (FAILED(hr))
        {
            goto done;
        }

        // Create the output node for the renderer.
        hr = AddOutputNode(pTopology, pSinkActivate, 0, &pOutputNode);
        if (FAILED(hr))
        {
            goto done;
        }

        // Connect the source node to the output node.
        hr = pSourceNode->ConnectOutput(0, pOutputNode, 0);
    }
    // else: If not selected, don't add the branch. 

done:
    SafeRelease(&pSD);
    SafeRelease(&pSinkActivate);
    SafeRelease(&pSourceNode);
    SafeRelease(&pOutputNode);
    return hr;
}

//  Create a playback topology from a media source.
HRESULT CreatePlaybackTopology(
    IMFMediaSource* pSource,          // Media source.
    IMFPresentationDescriptor* pPD,   // Presentation descriptor.
    HWND hVideoWnd,                   // Video window.
    IMFTopology** ppTopology)         // Receives a pointer to the topology.
{
    IMFTopology* pTopology = NULL;
    DWORD cSourceStreams = 0;

    // Create a new topology.
    HRESULT hr = MFCreateTopology(&pTopology);
    if (FAILED(hr))
    {
        goto done;
    }

    // Get the number of streams in the media source.
    hr = pPD->GetStreamDescriptorCount(&cSourceStreams);
    if (FAILED(hr))
    {
        goto done;
    }

    // For each stream, create the topology nodes and add them to the topology.
    for (DWORD i = 0; i < cSourceStreams; i++)
    {
        hr = AddBranchToPartialTopology(pTopology, pSource, pPD, i, hVideoWnd);
        if (FAILED(hr))
        {
            goto done;
        }
    }

    // Return the IMFTopology pointer to the caller.
    *ppTopology = pTopology;
    (*ppTopology)->AddRef();

done:
    SafeRelease(&pTopology);
    return hr;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0;          // GetImageEncoders�� ����
    UINT size = 0;         // GetImageEncoders�� ũ��

    // ���ڴ��� ������ ũ�⸦ ������
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1; // ���ڴ� ����

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1; // �޸� �Ҵ� ����

    // ��� ���ڴ� ������ ������
    GetImageEncoders(num, size, pImageCodecInfo);

    // ���ϴ� ������ CLSID�� �˻�
    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j; // CLSID�� ã��
        }
    }

    free(pImageCodecInfo);
    return -1; // CLSID�� ã�� ����
}

BYTE* NV12ToRGB32(const BYTE* pNV12, int width, int height)
{
    BYTE* pRGB = new BYTE[width * height * 4];
    int frameSize = width * height;
    const BYTE* pY = pNV12;                    // Y Plane ����
    const BYTE* pUV = pNV12 + frameSize;       // UV Plane ����

    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            int Y = pY[j * width + i];
            int U = pUV[(j / 2) * width + (i & ~1)] - 128; // UV�� 2x2 ���ø�
            int V = pUV[(j / 2) * width + (i & ~1) + 1] - 128;

            int R = Y + 1.402 * V;
            int G = Y - 0.344 * U - 0.714 * V;
            int B = Y + 1.772 * U;

            R = min(max(R, 0), 255);
            G = min(max(G, 0), 255);
            B = min(max(B, 0), 255);

            // RGB32 �ȼ� ���� (BGRA ����)
            int pixelIndex = (j * width + i) * 4;
            pRGB[pixelIndex] = B;  // Blue
            pRGB[pixelIndex + 1] = G;  // Green
            pRGB[pixelIndex + 2] = R;  // Red
            pRGB[pixelIndex + 3] = 255; // Alpha
        }
    }

    return pRGB;
}


HRESULT SeekToTime(IMFSourceReader* pReader, LONGLONG timeInHundredNanoSeconds) {
    PROPVARIANT var;
    PropVariantInit(&var);
    var.vt = VT_I8; // ������ �ð� �� ����
    var.hVal.QuadPart = timeInHundredNanoSeconds;

    HRESULT hr = pReader->SetCurrentPosition(GUID_NULL, var);
    PropVariantClear(&var);
    return hr;
}

// ������ ����� ���� �ð�
LONGLONG GetVideoCurrentTime(IMFSourceReader* pReader)
{
    if (!pReader)
        return -1; // ��ȿ���� ���� Reader ó��

    IMFSample* pSample = nullptr;
    DWORD dwStreamFlags = 0;
    DWORD dwStreamIndex = 0;
    LONGLONG llTimestamp = 0;
    HRESULT hr = pReader->ReadSample(
        MF_SOURCE_READER_FIRST_VIDEO_STREAM,
        0,
        &dwStreamIndex,
        &dwStreamFlags,
        &llTimestamp,
        &pSample
    );
    if (FAILED(hr) || !pSample) {
        MessageBox(NULL, L"�ð� �̵� ����", L"����", MB_OK);
        return -1; // ���� �б� ����
    }

    LONGLONG llTimeStamp = 0;
    hr = pSample->GetSampleTime(&llTimeStamp); // ���� �ð� ���� ��������
    pSample->Release();

    if (FAILED(hr)) {
        MessageBox(NULL, L"�ð� �̵� ����", L"����", MB_OK);
        return -1; // �ð� ���� �������� ����
    }

    return llTimeStamp; // ��ȯ�� ���� 100-������ ����
}

// ���� ��ü �ð�
LONGLONG GetVideoTime(IMFSourceReader* pReader) {
    PROPVARIANT varPosition;
    PropVariantInit(&varPosition);

    HRESULT hr = pReader->GetPresentationAttribute(
        MF_SOURCE_READER_MEDIASOURCE,    // �̵�� �ҽ��� ���� �Ӽ�
        MF_PD_DURATION,                  // ��� �ð� �Ӽ�
        &varPosition                     // �Ӽ� �� ����
    );

    LONGLONG time = 0;
    if (SUCCEEDED(hr)) {
        if (varPosition.vt == VT_UI8) {
            time = varPosition.uhVal.QuadPart;
        }
        else {
            hr = E_FAIL;  // �߸��� ������ ����
            MessageBox(NULL, L"������ ���� �ð� �б� ����", L"����", MB_OK);
        }
    }

    PropVariantClear(&varPosition);
    return time;
}

HRESULT GetVideoResolution(IMFSourceReader* pReader, UINT32* pWidth, UINT32* pHeight)
{
    IMFMediaType* pNativeType = NULL;
    HRESULT hr;

    IMFMediaType* pMediaType = nullptr;
    hr = pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pMediaType);
    if (FAILED(hr))
        return hr;

    hr = MFGetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, pWidth, pHeight);
    pMediaType->Release();

    return hr;
}


GUID GetVideoFormat(IMFSourceReader* pSourceReader)
{
    IMFMediaType* pMediaType = nullptr;

    GUID subtype;
    // ���� ��Ʈ���� Media Type �������� (Stream Index 0 ����)
    HRESULT hr = pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pMediaType);
    if (FAILED(hr)) {
        std::cerr << "Failed to get video media type" << std::endl;
        return subtype;
    }

    // ���� ����
    if (SUCCEEDED(pMediaType->GetGUID(MF_MT_SUBTYPE, &subtype))) {
        std::wcout << L"Video Format: " << subtype.Data4 << std::endl;
    }

    if (pMediaType) pMediaType->Release();
    return subtype;
}

double ConvertNanoSecondsToSeconds(LONGLONG time100ns) {
    return static_cast<double>(time100ns) / 10'000'000.0;  // 100ns �� ��
}

LONGLONG ConvertSecondsToNanoSeconds(double seconds) {
    return static_cast<LONGLONG>(seconds) * 10'000'000.0;  // 100ns �� ��
}

DWORD GetStreamLength(IMFSourceReader* pReader)
{
    HRESULT hr = S_OK;
    DWORD dwStreamIndex = 0;

    while (true)
    {
        IMFMediaType* pMediaType = NULL;

        // �� ��Ʈ���� �̵�� Ÿ���� ������
        hr = pReader->GetNativeMediaType(dwStreamIndex, 0, &pMediaType);
        if (FAILED(hr))
        {
            // �� �̻� ��Ʈ���� ���� ��� ���� ����
            if (hr == MF_E_NO_MORE_TYPES)
            {
                hr = S_OK;
                break;
            }
            else
            {
                break;
            }
        }

        GUID majorType;
        hr = pMediaType->GetGUID(MF_MT_MAJOR_TYPE, &majorType);
        pMediaType->Release();

        if (FAILED(hr))
        {
            break;
        }

        dwStreamIndex++;  // ���� ��Ʈ������ �̵�
    }
    return dwStreamIndex;
}

HRESULT FindVideoStreamIndex(IMFSourceReader* pReader, DWORD* pdwVideoStreamIndex)
{
    HRESULT hr = S_OK;
    DWORD dwStreamIndex = 0;
    *pdwVideoStreamIndex = (DWORD)-1;  // �⺻���� -1�� ���� (���� ��Ʈ���� ã�� ���� ���)

    while (true)
    {
        IMFMediaType* pMediaType = NULL;

        // GetNativeMediaType�� ����Ͽ� ��Ʈ���� �̵�� Ÿ���� ������
        hr = pReader->GetNativeMediaType(dwStreamIndex, 0, &pMediaType);
        if (FAILED(hr))
        {
            break;  // �� �̻� ��Ʈ���� �����Ƿ� ����
        }

        GUID majorType;
        hr = pMediaType->GetGUID(MF_MT_MAJOR_TYPE, &majorType);
        pMediaType->Release();

        if (FAILED(hr))
        {
            break;
        }

        // ��Ʈ���� ���� Ÿ������ Ȯ��
        if (majorType == MFMediaType_Video)
        {
            *pdwVideoStreamIndex = dwStreamIndex;
            return S_OK;  // ���� ��Ʈ���� ã�����Ƿ� �ε����� ��ȯ
        }

        dwStreamIndex++;  // ���� ��Ʈ������ �Ѿ
    }

    // ���� ��Ʈ���� ã�� ���� ���
    return E_FAIL;
}

HRESULT ConfigureVideoDecoder(IMFSourceReader* pReader)
{
    HRESULT hr;
    IMFMediaType* pNativeType = NULL;
    IMFMediaType* pType = NULL;

    // Find the native format of the stream.
    hr = pReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &pNativeType);
    if (FAILED(hr))
    {
        return hr;
    }

    GUID majorType, subtype;

    // Find the major type.
    hr = pNativeType->GetGUID(MF_MT_MAJOR_TYPE, &majorType);
    if (FAILED(hr))
    {
        goto done;
    }

    // Define the output type.
    hr = MFCreateMediaType(&pType);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pType->SetGUID(MF_MT_MAJOR_TYPE, majorType);
    if (FAILED(hr))
    {
        goto done;
    }

    //MFVideoFormat_NV12
    hr = pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);
    if (FAILED(hr))
    {
        goto done;
    }

    UINT32 w, h;
    hr = GetVideoResolution(pReader, &w, &h);
    if (FAILED(hr)) goto done;

    hr = MFSetAttributeSize(pType, MF_MT_FRAME_SIZE, w, h);
    if (FAILED(hr)) goto done;

    // Set the uncompressed format.
    hr = pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pType);
    if (FAILED(hr))
    {
        goto done;
    }

done:
    SafeRelease(&pNativeType);
    SafeRelease(&pType);
    return hr;
}

HRESULT CreateSourceReaderAsync(
    PCWSTR pszURL,
    IMFSourceReaderCallback* pCallback,
    IMFSourceReader** ppReader)
{
    HRESULT hr = S_OK;
    IMFAttributes* pAttributes = NULL;

    hr = MFCreateAttributes(&pAttributes, 1);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pAttributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, pCallback);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = MFCreateSourceReaderFromURL(pszURL, pAttributes, ppReader);

done:
    SafeRelease(&pAttributes);
    return hr;
}

HRESULT GetVideoResolution(IMFMediaSource* pSource, UINT32& width, UINT32& height)
{
    if (!pSource) return E_POINTER;

    IMFPresentationDescriptor* pPD = nullptr;
    IMFStreamDescriptor* pSD = nullptr;
    IMFMediaTypeHandler* pHandler = nullptr;
    IMFMediaType* pMediaType = nullptr;

    HRESULT hr = pSource->CreatePresentationDescriptor(&pPD);
    if (FAILED(hr)) return hr;

    BOOL fSelected = FALSE;
    DWORD cStreams = 0;

    // Get the number of streams
    hr = pPD->GetStreamDescriptorCount(&cStreams);
    if (FAILED(hr)) goto done;

    for (DWORD i = 0; i < cStreams; ++i)
    {
        // Get the stream descriptor
        hr = pPD->GetStreamDescriptorByIndex(i, &fSelected, &pSD);
        if (FAILED(hr)) goto done;

        // Check if this is a video stream
        hr = pSD->GetMediaTypeHandler(&pHandler);
        if (FAILED(hr)) goto done;

        GUID majorType;
        hr = pHandler->GetMajorType(&majorType);
        if (FAILED(hr)) goto done;

        if (majorType == MFMediaType_Video)
        {
            // Get the current media type
            hr = pHandler->GetCurrentMediaType(&pMediaType);
            if (FAILED(hr)) goto done;

            // Get the frame size
            UINT64 frameSize = 0;
            hr = pMediaType->GetUINT64(MF_MT_FRAME_SIZE, &frameSize);
            if (FAILED(hr)) goto done;

            // Extract width and height
            width = static_cast<UINT32>(frameSize & 0xFFFFFFFF);       // Low 32 bits
            height = static_cast<UINT32>((frameSize >> 32) & 0xFFFFFFFF); // High 32 bits
            break; // Found the video stream
        }

        // Release resources for this stream
        SafeRelease(&pSD);
        SafeRelease(&pHandler);
    }

done:
    SafeRelease(&pPD);
    SafeRelease(&pSD);
    SafeRelease(&pHandler);
    SafeRelease(&pMediaType);

    return hr;
}
