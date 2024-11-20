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
        // FFmpeg ������ ���� ���μ��� ����
        PROCESS_INFORMATION pi;
        STARTUPINFOW si{ sizeof(STARTUPINFOW) };
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;  // FFmpeg ���� â �����

        if (!CreateProcessW(
            NULL,                    // ���ø����̼� ��� (NULL�� ��ɾ�� ���)
            const_cast<LPWSTR>(command.c_str()), // ��ɾ� (LPSTR �������� ��ȯ)
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
