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

void GIF::SetTime(std::pair<float, float> times)
{
    startTime = times.first;
    endTime = times.second;
}

int GIF::Make(std::wstring& inputFile, std::wstring& outputFile)
{
    std::wstring curInputPath = inputFile;
    std::wstring curPalettePath = outputFile;
    std::wstring curOutputPath = outputFile;

    WrapingQuotes(curInputPath);
    curPalettePath.append(L".png");
    curOutputPath.append(L".gif");
    WrapingQuotes(curPalettePath);
    WrapingQuotes(curOutputPath);

    std::wstring startTimeStr = TimeFormat(startTime);
    std::wstring endTimeStr = TimeFormat(endTime);

    std::wstring paletteCommand =
        currentDirPath + L"\\ffmpeg.exe" +
        L" -ss " + startTimeStr +
        L" -to " + endTimeStr +
        L" -i " + curInputPath + 
        L" -vf \"fps = 30, scale = 1280:-1 : flags = lanczos, palettegen\" "+ 
        curPalettePath;
    std::wstring gifCommand =
        currentDirPath + L"\\ffmpeg.exe" +
        L" -ss " + startTimeStr +
        L" -to " + endTimeStr +
        L" -i " + curInputPath + 
        L" -i " + curPalettePath +
        L" -filter_complex \"fps = 30, scale = 1280:-1 : flags = lanczos[x]; [x] [1:v] paletteuse\" " +curOutputPath;
    std::thread makeThread(&GIF::MakeThread, *this, paletteCommand, gifCommand);

    makeThread.detach();
    return 0;
}

std::wstring GIF::TimeFormat(const float seconds)
{
    int totalSeconds = static_cast<int>(seconds);  // ���� ��
    int hours = totalSeconds / 3600;              // �� ���
    int minutes = (totalSeconds % 3600) / 60;     // �� ���
    int secs = totalSeconds % 60;                 // �� ���
    int milliseconds = static_cast<int>((seconds - totalSeconds) * 1000); // �и��� ���

    std::wostringstream oss;
    oss << std::setw(2) << std::setfill(L'0') << hours << L":"
        << std::setw(2) << std::setfill(L'0') << minutes << L":"
        << std::setw(2) << std::setfill(L'0') << secs << L"."
        << std::setw(3) << std::setfill(L'0') << milliseconds;
    return oss.str();
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
