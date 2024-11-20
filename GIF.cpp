#include "GIF.h"

GIF::GIF()
{
    gifViewPosition.x = 0;
    gifViewPosition.y = 0;

    gifViewSize.x = 100;
    gifViewSize.y = 100;

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

GIF::~GIF()
{
    Release();
}

void GIF::Release()
{
    delete gifImage;
    gifImage = nullptr;
    delete frameDimension;
}

void GIF::LoadGIF(const WCHAR* _filepath)
{
    gifImage = new Image(_filepath);
    if (!gifImage) return;

    *filepath = *_filepath;

    // 프레임 정보 가져오기
    UINT count = gifImage->GetFrameDimensionsCount();
    frameDimension = new GUID[count];
    gifImage->GetFrameDimensionsList(frameDimension, count);

    // GIF의 총 프레임 수
    frameCount = gifImage->GetFrameCount(&frameDimension[0]);
}

void GIF::UpdateGIFFrame(HWND hWnd)
{
    currentFrame = (currentFrame + 1) % frameCount;
    gifImage->SelectActiveFrame(&frameDimension[0], currentFrame);
    InvalidateRect(hWnd, NULL, TRUE); // 화면 갱신 요청
}

int GIF::Make(std::wstring& inputFile, std::wstring& outputFile) const
{
    std::wstring curInputPath = inputFile;
    std::wstring curPalettePath = outputFile;
    std::wstring curOutputPath = outputFile;

    WrapingQuotes(curInputPath);
    curPalettePath.append(L".png");
    curOutputPath.append(L".gif");
    WrapingQuotes(curPalettePath);
    WrapingQuotes(curOutputPath);

    std::wstring paletteCommand = currentDirPath;
    paletteCommand.append(L"\\ffmpeg.exe -i ").append(curInputPath).append(L" -vf \"fps = 30, scale = 1280:-1 : flags = lanczos, palettegen\" ").append(curPalettePath);
    std::wstring gifCommand = currentDirPath;
    gifCommand.append(L"\\ffmpeg.exe -i ").append(curInputPath).append(L" -i ").append(curPalettePath).append(L" -filter_complex \"fps = 30, scale = 1280:-1 : flags = lanczos[x]; [x] [1:v] paletteuse\" ").append(curOutputPath);
    std::thread makeThread(&GIF::MakeThread, *this, paletteCommand, gifCommand);

    makeThread.detach();
    return 0;
}

int GIF::MakeThread(const GIF& gif, std::wstring paletteCommand, std::wstring gifCommand)
{
    auto process = [](const std::wstring& command)
    {
        // FFmpeg 실행을 위한 프로세스 생성
        PROCESS_INFORMATION pi;
        STARTUPINFOW si{ sizeof(STARTUPINFOW) };
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;  // FFmpeg 실행 창 숨기기

        if (!CreateProcessW(
            NULL,                    // 애플리케이션 경로 (NULL은 명령어로 사용)
            const_cast<LPWSTR>(command.c_str()), // 명령어 (LPSTR 형식으로 변환)
            NULL,                    // 프로세스 보안 속성
            NULL,                    // 스레드 보안 속성
            FALSE,                   // 표준 핸들 상속 여부
            0,                       // 실행 옵션
            NULL,                    // 환경 변수
            NULL,                    // 현재 디렉토리
            &si,                     // STARTUPINFOA 구조체
            &pi                      // PROCESS_INFORMATION 구조체
        )) {
            std::cerr << "Failed to create process!" << std::endl;
            return -1;
        }
        WaitForSingleObject(pi.hProcess, INFINITE);

        // 리소스 해제
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    };
    process(paletteCommand);
    gif.paletteGenerateEvent.Execute<void>();

    process(gifCommand);
    gif.gifGenerateEvent.Execute<void>();

    std::cout << "GIF creation finished!" << std::endl;
    return 0;
}
