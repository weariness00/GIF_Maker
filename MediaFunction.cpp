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

HRESULT ConvertToRGB32(IMFMediaBuffer* pBuffer, UINT32 width, UINT32 height)
{
    HRESULT hr = S_OK;

    // 비디오 포맷을 32bpp RGB로 설정
    GUID guidOutputFormat = MFVideoFormat_RGB32;  // 32bpp RGB 포맷

    // pMediaType 생성 (MediaType는 비디오 포맷을 지정하는 객체)
    IMFMediaType* pMediaType = nullptr;
    hr = MFCreateMediaType(&pMediaType);  // IMFMediaType 객체 생성
    if (FAILED(hr)) {
        return hr;
    }

    // MediaType의 Subtype을 32bpp RGB로 설정
    hr = pMediaType->SetGUID(MF_MT_SUBTYPE, guidOutputFormat);
    if (FAILED(hr)) {
        pMediaType->Release();  // 실패하면 메모리 해제
        return hr;
    }

    // 비디오 프레임 크기 및 다른 파라미터 설정
    hr = pMediaType->SetUINT32(MF_MT_AVG_BITRATE, width * height * 4);  // 비디오 크기에 따라 비트레이트 설정 (예시)
    if (FAILED(hr)) {
        pMediaType->Release();
        return hr;
    }

    // 변환기를 사용하여 비디오 포맷을 변환하려면 MFT(Media Foundation Transform)가 필요합니다.
    IMFTransform* pTransform = nullptr;
    hr = MFCreateTransformActivate(&pTransform);
    if (FAILED(hr)) {
        pMediaType->Release();
        return hr;
    }

    // 변환기의 입력 포맷을 설정
    hr = pTransform->SetInputType(0, pMediaType, 0);
    if (FAILED(hr)) {
        pMediaType->Release();
        pTransform->Release();
        return hr;
    }

    // 변환기에서 출력 타입을 설정합니다.
    hr = pTransform->SetOutputType(0, pMediaType, 0);
    if (FAILED(hr)) {
        pMediaType->Release();
        pTransform->Release();
        return hr;
    }

    // 버퍼에서 데이터를 읽고 변환을 진행합니다.
    // 이 부분은 실제로 변환된 비디오 데이터를 처리하는 부분입니다.
    // 예시로는 변환된 프레임을 새로운 버퍼로 저장하거나 출력할 수 있습니다.
    
    // 자원 해제
    pMediaType->Release();
    pTransform->Release();

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
            MessageBox(NULL, L"스트림 끝에 도달했습니다.", L"정보", MB_OK);
        }
        else {
            MessageBox(NULL, L"프레임 읽기 실패", L"오류", MB_OK);
        }
        return nullptr;
    }

    hr = pSample->ConvertToContiguousBuffer(&pBuffer);
    if (FAILED(hr)) {
        MessageBox(NULL, L"버퍼 변환 실패", L"오류", MB_OK);
        pSample->Release();
        return nullptr;
    }

    hr = pBuffer->Lock(&pData, nullptr, &dwBufferSize);
    if (FAILED(hr)) {
        MessageBox(NULL, L"버퍼 잠금 실패", L"오류", MB_OK);
        pBuffer->Release();
        pSample->Release();
        return nullptr;
    }

    // 프레임의 크기를 가져옵니다.
    IMFMediaType* pMediaType = nullptr;
    pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pMediaType);
    // 비디오 포맷 확인
    GUID subtype;
    hr = pMediaType->GetGUID(MF_MT_SUBTYPE, &subtype);
    if (FAILED(hr)) {
        pMediaType->Release();
        return nullptr;
    }

    if (subtype == MFVideoFormat_RGB32) {
        MessageBox(NULL, L"비디오 포맷은 32bpp RGB입니다.", L"정보", MB_OK);
    }
    else {
        MessageBox(NULL, L"비디오 포맷이 32bpp RGB가 아닙니다.", L"정보", MB_OK);
    }

    MFGetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, &width, &height);
    pMediaType->Release();

    auto i = width * height;
    // 복사 후 해제
    BYTE* pFrameData = new BYTE[dwBufferSize];
    memcpy(pFrameData, pData, dwBufferSize);

    pBuffer->Unlock();
    pBuffer->Release();
    pSample->Release();

    return pFrameData;
}

BYTE* ReadFrameAtTime(IMFSourceReader* pReader, LONGLONG timeInHundredNanoSeconds, UINT32& width, UINT32& height) {
    // 특정 시간으로 이동
    HRESULT hr;
    LONGLONG pRenderCurrentTime = GetVideoCurrentTime(pReader);
	hr = SeekToTime(pReader, timeInHundredNanoSeconds);
    if (FAILED(hr)) {
        MessageBox(NULL, L"시간 이동 실패", L"오류", MB_OK);
        width = -1;
        height = -1;
    	return nullptr;
    }

    // 프레임 읽기
    BYTE* frameByte = ReadFrame(pReader, width, height);
    SeekToTime(pReader, pRenderCurrentTime);

    return frameByte;
}

Gdiplus::Bitmap* MakeBitmapToFrame(BYTE* frameByte, const UINT& width, const UINT& height)
{
    return new Gdiplus::Bitmap(
        width,            // 너비 (비디오의 해상도에 맞춰 변경)
        height,            // 높이
        width * 4,        // 스트라이드 (너비 × 4)
        PixelFormat32bppRGB, // 픽셀 포맷
        frameByte            // 버퍼 데이터
    );
}


HRESULT SeekToTime(IMFSourceReader* pReader, LONGLONG timeInHundredNanoSeconds) {
    PROPVARIANT var;
    PropVariantInit(&var);
    var.vt = VT_I8; // 정수형 시간 값 설정
    var.hVal.QuadPart = timeInHundredNanoSeconds;

    HRESULT hr = pReader->SetCurrentPosition(GUID_NULL, var);
    PropVariantClear(&var);
    return hr;
}

// 비디오의 재생된 현재 시간
LONGLONG GetVideoCurrentTime(IMFSourceReader* pReader)
{
    if (!pReader)
        return -1; // 유효하지 않은 Reader 처리

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
        MessageBox(NULL, L"시간 이동 실패", L"오류", MB_OK);
        return -1; // 샘플 읽기 실패
    }

    LONGLONG llTimeStamp = 0;
    hr = pSample->GetSampleTime(&llTimeStamp); // 현재 시간 정보 가져오기
    pSample->Release();

    if (FAILED(hr)) {
        MessageBox(NULL, L"시간 이동 실패", L"오류", MB_OK);
        return -1; // 시간 정보 가져오기 실패
    }

    return llTimeStamp; // 반환된 값은 100-나노초 단위
}

// 비디오 전체 시간
LONGLONG GetVideoTime(IMFSourceReader* pReader) {
    PROPVARIANT varPosition;
    PropVariantInit(&varPosition);

    HRESULT hr = pReader->GetPresentationAttribute(
        MF_SOURCE_READER_MEDIASOURCE,    // 미디어 소스에 대한 속성
        MF_PD_DURATION,                  // 재생 시간 속성
        &varPosition                     // 속성 값 저장
    );

    LONGLONG time = 0;
    if (SUCCEEDED(hr)) {
        if (varPosition.vt == VT_UI8) {
            time = varPosition.uhVal.QuadPart;
        }
        else {
            hr = E_FAIL;  // 잘못된 데이터 형식
            MessageBox(NULL, L"비디오의 현재 시간 읽기 실패", L"오류", MB_OK);
        }
    }

    PropVariantClear(&varPosition);
    return time;
}

double ConvertNanoSecondsToSeconds(LONGLONG time100ns) {
    return static_cast<double>(time100ns) / 10'000'000.0;  // 100ns → 초
}

LONGLONG ConvertSecondsToNanoSeconds(double seconds) {
    return static_cast<LONGLONG>(seconds) * 10'000'000.0;  // 100ns → 초
}

// PNG 인코더의 CLSID를 얻는 함수
CLSID* GetEncoderClsid(const WCHAR* format) {
    UINT numEncoders = 0;       // 인코더 수
    UINT size = 0;             // 인코더 정보의 크기

    // GDI+에서 인코더 정보를 얻음
    Gdiplus::GetImageEncodersSize(&numEncoders, &size);
    if (size == 0) return nullptr;  // 인코더 정보가 없으면 종료

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return nullptr; // 메모리 할당 실패

    GetImageEncoders(numEncoders, size, pImageCodecInfo);

    // PNG 인코더의 CLSID 찾기
    CLSID* pClsid = nullptr;
    for (UINT i = 0; i < numEncoders; ++i) {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
            // CLSID 찾음
        	pClsid = &pImageCodecInfo[i].Clsid;
            break;  
        }
    }

    free(pImageCodecInfo);
    return pClsid;
}