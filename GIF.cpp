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

int GIF::Make(const char* inputFile, const char* outputFile) const
{
    auto curInputPath = WrapingQuotes(inputFile);
    auto curPalettePath = WrapingQuotes( std::string(outputFile) + ".png");
    auto curOutputPath = WrapingQuotes(std::string(outputFile) + ".gif");
    std::string paletteCommand = currentDirPath + "\\ffmpeg.exe -i " + curInputPath + " -vf \"fps = 30, scale = 1280:-1 : flags = lanczos, palettegen\" " + curPalettePath;
    std::string gifCommand = currentDirPath + "\\ffmpeg.exe -i " + curInputPath + " -i " + curPalettePath + " -filter_complex \"fps = 30, scale = 1280:-1 : flags = lanczos[x]; [x] [1:v] paletteuse\" " + curOutputPath;
    std::thread makeThread(&GIF::MakeThread, *this, paletteCommand, gifCommand);

    makeThread.detach();
    return 0;
}

int GIF::MakeThread(const GIF& gif, const std::string paletteCommand, const std::string gifCommand)
{
    auto process = [](const std::string& command)
    {
        // FFmpeg 실행을 위한 프로세스 생성
        PROCESS_INFORMATION pi;
        STARTUPINFOA si = { sizeof(STARTUPINFOA) };  // STARTUPINFOA 구조체 초기화
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;  // FFmpeg 실행 창 숨기기

        // CreateProcessA 함수 호출
        if (!CreateProcessA(
            NULL,                    // 애플리케이션 경로 (NULL은 명령어로 사용)
            const_cast<LPSTR>(command.c_str()), // 명령어 (LPSTR 형식으로 변환)
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
