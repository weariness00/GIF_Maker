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

    // ������ ���� ��������
    UINT count = gifImage->GetFrameDimensionsCount();
    frameDimension = new GUID[count];
    gifImage->GetFrameDimensionsList(frameDimension, count);

    // GIF�� �� ������ ��
    frameCount = gifImage->GetFrameCount(&frameDimension[0]);
}

void GIF::UpdateGIFFrame(HWND hWnd)
{
    currentFrame = (currentFrame + 1) % frameCount;
    gifImage->SelectActiveFrame(&frameDimension[0], currentFrame);
    InvalidateRect(hWnd, NULL, TRUE); // ȭ�� ���� ��û
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
        // FFmpeg ������ ���� ���μ��� ����
        PROCESS_INFORMATION pi;
        STARTUPINFOA si = { sizeof(STARTUPINFOA) };  // STARTUPINFOA ����ü �ʱ�ȭ
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;  // FFmpeg ���� â �����

        // CreateProcessA �Լ� ȣ��
        if (!CreateProcessA(
            NULL,                    // ���ø����̼� ��� (NULL�� ��ɾ�� ���)
            const_cast<LPSTR>(command.c_str()), // ��ɾ� (LPSTR �������� ��ȯ)
            NULL,                    // ���μ��� ���� �Ӽ�
            NULL,                    // ������ ���� �Ӽ�
            FALSE,                   // ǥ�� �ڵ� ��� ����
            0,                       // ���� �ɼ�
            NULL,                    // ȯ�� ����
            NULL,                    // ���� ���丮
            &si,                     // STARTUPINFOA ����ü
            &pi                      // PROCESS_INFORMATION ����ü
        )) {
            std::cerr << "Failed to create process!" << std::endl;
            return -1;
        }
        WaitForSingleObject(pi.hProcess, INFINITE);

        // ���ҽ� ����
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
