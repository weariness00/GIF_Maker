﻿// WindowsProject1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "WindowsProject1.h"
#include "WindowExplorer.h"
#include "GIF.h"
#include "VideoView.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
ProjectManager pm;

/* GDI 관련 데이터 */

ULONG_PTR gdiplusToken;
GdiplusStartupInput gdiplusStartupInput;

GIF testGIF;
VideoView* videoView;
VideoTimeLineView* videoTimeLineView;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void Init()
{
    // GDI+ 관련된 어떤 함수라도 사용 전에 해당 함수를 호출해야 합니다.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    currentDirPath = std::filesystem::current_path().wstring();
    auto dirPath = std::filesystem::current_path() / PreviewDirPath;
    std::filesystem::create_directory(dirPath);
    PreviewDirPath = dirPath.wstring();

    testGIF.paletteGenerateEvent.AddEvent(std::function<void()>([]()
        {
            std::thread massageBoxThread([]()
            {
            	return MessageBoxW(NULL, L"성공적으로 palette 생성 완료", L"File Path", MB_OK);
            });
            massageBoxThread.detach();
        }));

    testGIF.gifGenerateEvent.AddEvent(std::function<void()>([]()
        {
            std::thread massageBoxThread([]()
                {
                    return MessageBoxW(NULL, L"성공적으로 gif 생성 완료", L"File Path", MB_OK);
                });
            massageBoxThread.detach();
        }));
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
    Init();

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
			
    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        for (auto object : WindowObject::objects)
        {
            if (object->GetParent()) continue;
            object->UpdateWindowTransform(nullptr);
        }

        for (auto object : WindowObject::objects)
        {
            object->Update();
        }

        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowExW(
       0,                             // 확장 스타일 (기본적으로 0으로 설정)
       szWindowClass,                 // 윈도우 클래스 이름
       szTitle,                       // 윈도우 타이틀
       WS_OVERLAPPEDWINDOW,           // 윈도우 스타일
       CW_USEDEFAULT,                 // x 좌표
       0,                             // y 좌표
       CW_USEDEFAULT,                 // 너비
       0,                             // 높이
       nullptr,                       // 부모 윈도우 핸들
       nullptr,                       // 메뉴 핸들
       hInstance,                     // 인스턴스 핸들
       nullptr                        // 추가 데이터
   );

   //HWND hWnd = CreateWindowW(
   //    szWindowClass, 
   //    szTitle,
   //    WS_OVERLAPPEDWINDOW,
   //    CW_USEDEFAULT,
   //    0, 
   //    CW_USEDEFAULT, 
   //    0, 
   //    nullptr, 
   //    nullptr, 
   //    hInstance, 
   //    nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:{
        RECT rect;
        GetClientRect(hWnd, &rect);
        int w = rect.right - rect.left;
        videoView = new VideoView(hWnd);
        videoTimeLineView = new VideoTimeLineView(
            hWnd,
            { 0,400, w, 400 });
        break;
    }
    //case WM_LBUTTONDOWN:
    //    break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case HIGH_QUALITY_GIF:{
                auto outputFile = PreviewDirPath + L"\\임시";
                testGIF.SetTime(videoTimeLineView->GetTime());
                testGIF.Make(*videoView->GetVideoPath(), outputFile);
                break;
            }
            case WindowFileLoad:
                ProjectManager::Instance->videoOpenExploprer.FileOpenDialog(hWnd);
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    //case WM_CHAR:
    //    videoView->OnKeyPress(wParam);
    //    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}