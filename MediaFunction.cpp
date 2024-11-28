#include "MediaFunction.h"

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

BYTE* ReadFrame(IMFSourceReader* pReader, UINT32& width, UINT32& height) {
    IMFSample* pSample = nullptr;
    IMFMediaBuffer* pBuffer = nullptr;
    BYTE* pData = nullptr;
    DWORD dwBufferSize = 0;

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
        if (hr == MF_E_END_OF_STREAM) {
            MessageBox(NULL, L"��Ʈ�� ���� �����߽��ϴ�.", L"����", MB_OK);
        }
        else {
            MessageBox(NULL, L"������ �б� ����", L"����", MB_OK);
        }
        return nullptr;
    }

    hr = pSample->ConvertToContiguousBuffer(&pBuffer);
    if (FAILED(hr)) {
        MessageBox(NULL, L"���� ��ȯ ����", L"����", MB_OK);
        pSample->Release();
        return nullptr;
    }

    hr = pBuffer->Lock(&pData, nullptr, &dwBufferSize);
    if (FAILED(hr)) {
        MessageBox(NULL, L"���� ��� ����", L"����", MB_OK);
        pBuffer->Release();
        pSample->Release();
        return nullptr;
    }

    // �������� ũ�⸦ �����ɴϴ�.
    IMFMediaType* pMediaType = nullptr;
    pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pMediaType);
    // ���� ���� Ȯ��
    GUID subtype;
    hr = pMediaType->GetGUID(MF_MT_SUBTYPE, &subtype);
    if (FAILED(hr)) {
        pMediaType->Release();
        return nullptr;
    }

    if (subtype == MFVideoFormat_RGB32) {
        MessageBox(NULL, L"���� ������ 32bpp RGB�Դϴ�.", L"����", MB_OK);
    }
    else {
        MessageBox(NULL, L"���� ������ 32bpp RGB�� �ƴմϴ�.", L"����", MB_OK);
    }

    MFGetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, &width, &height);
    pMediaType->Release();

    auto i = width * height;
    // ���� �� ����
    BYTE* pFrameData = new BYTE[dwBufferSize];
    memcpy(pFrameData, pData, dwBufferSize);

    pBuffer->Unlock();
    pBuffer->Release();
    pSample->Release();

    return pFrameData;
}

BYTE* ReadFrameAtTime(IMFSourceReader* pReader, LONGLONG timeInHundredNanoSeconds, UINT32& width, UINT32& height) {
    // Ư�� �ð����� �̵�
    HRESULT hr;
    LONGLONG pRenderCurrentTime = GetVideoCurrentTime(pReader);
	hr = SeekToTime(pReader, timeInHundredNanoSeconds);
    if (FAILED(hr)) {
        MessageBox(NULL, L"�ð� �̵� ����", L"����", MB_OK);
        width = -1;
        height = -1;
    	return nullptr;
    }

    // ������ �б�
    BYTE* frameByte = ReadFrame(pReader, width, height);
    SeekToTime(pReader, pRenderCurrentTime);

    return frameByte;
}

Gdiplus::Bitmap* MakeBitmapToFrame(BYTE* frameByte, const UINT& width, const UINT& height)
{
    return new Gdiplus::Bitmap(
        width,            // �ʺ� (������ �ػ󵵿� ���� ����)
        height,            // ����
        width * 4,        // ��Ʈ���̵� (�ʺ� �� 4)
        PixelFormat32bppRGB, // �ȼ� ����
        frameByte            // ���� ������
    );
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

double ConvertNanoSecondsToSeconds(LONGLONG time100ns) {
    return static_cast<double>(time100ns) / 10'000'000.0;  // 100ns �� ��
}

LONGLONG ConvertSecondsToNanoSeconds(double seconds) {
    return static_cast<LONGLONG>(seconds) * 10'000'000.0;  // 100ns �� ��
}

// PNG ���ڴ��� CLSID�� ��� �Լ�
CLSID* GetEncoderClsid(const WCHAR* format) {
    UINT numEncoders = 0;       // ���ڴ� ��
    UINT size = 0;             // ���ڴ� ������ ũ��

    // GDI+���� ���ڴ� ������ ����
    Gdiplus::GetImageEncodersSize(&numEncoders, &size);
    if (size == 0) return nullptr;  // ���ڴ� ������ ������ ����

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return nullptr; // �޸� �Ҵ� ����

    GetImageEncoders(numEncoders, size, pImageCodecInfo);

    // PNG ���ڴ��� CLSID ã��
    CLSID* pClsid = nullptr;
    for (UINT i = 0; i < numEncoders; ++i) {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
            // CLSID ã��
        	pClsid = &pImageCodecInfo[i].Clsid;
            break;  
        }
    }

    free(pImageCodecInfo);
    return pClsid;
}