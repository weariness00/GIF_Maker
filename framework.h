// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "MainView.h"
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
#include <windowsx.h>
// C 런타임 헤더 파일입니다.
#include <filesystem>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>
#include <shobjidl.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <propvarutil.h>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>
#include <iomanip>

#include "GDIPlusManager.h"
#include "TDelegate.h"
#include "WindowObject.h"
#include "VideoPlayer.h"
#include "VideoCaptureController.h"
#include "ImageController.h"
#include "BitmapController.h"
#include "TextController.h"
#include "DoubleBufferingWindow.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "propsys.lib")

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "windowscodecs.lib")

inline std::wstring currentDirPath;
inline std::wstring PreviewDirPath = L"Preview";

// 문자열을 따음표로 감싸기
// www\dddds\d d.gif -> "www\dddds\d d.gif"
inline std::wstring WrapingQuotes(const std::wstring& s)
{
    std::wstring newStr = L"\"";
    newStr.append(s);
    newStr.append(L"\"");
    return newStr;
}

inline HWND CreateIndependentWindow(HINSTANCE hInstance, std::wstring& windowTitle) {
    // 윈도우 클래스 등록
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = DefWindowProc; // 기본 메시지 처리기
    wc.hInstance = hInstance;
    wc.lpszClassName = L"IndependentWindow";

    RegisterClass(&wc);

    // 독립적인 윈도우 생성
    HWND hWnd = CreateWindowW(
        L"IndependentWindow", // 클래스 이름
        windowTitle.c_str(), // 윈도우 타이틀
        WS_OVERLAPPEDWINDOW, // 독립적인 스타일 (타이틀바, 테두리 포함)
        CW_USEDEFAULT, CW_USEDEFAULT, // 기본 위치
        500, 500, // 너비와 높이
        NULL, // 부모 없음
        NULL, // 메뉴 없음
        hInstance, // 애플리케이션 핸들
        NULL // 추가 데이터 없음
    );

    if (hWnd) {
        ShowWindow(hWnd, SW_SHOW); // 윈도우를 화면에 표시
    }

    return hWnd;
}

struct TestS
{
    int a;
};
// 독립적이지 않은 윈도우를 생성하는 함수
inline HWND CreateChildWindow(
    HWND hParent,                       // 부모 윈도우 핸들
    HINSTANCE hInstance,                // 인스턴스 핸들
    LPCWSTR className,                  // 자식 윈도우 클래스 이름
    WNDPROC childWndProc,               // 자식 윈도우 프로시저
    RECT& rect, // 자식 윈도우 위치 및 크기
    LPVOID data = nullptr,
    COLORREF color = RGB( 0,0,255 )
) {

    std::wstring ws = className;
    size_t lastLineStart = ws.rfind(L"\0");
    ws.append(L"_0");
    // 윈도우 클래스 등록
    // 자식 윈도우 클래스 등록
    WNDCLASSW wcChild = {};
    wcChild.lpfnWndProc = childWndProc;
    wcChild.hInstance = hInstance;
    wcChild.lpszClassName = ws.c_str();
    wcChild.hbrBackground = CreateSolidBrush(color);

    int count = 1;
    while(GetClassInfoW(hInstance, ws.c_str(), &wcChild)) {
        ws.assign(ws.substr(0, lastLineStart) + L"_" + std::to_wstring(count));
        wcChild.lpszClassName = ws.c_str();
        ++count;
    }

    if (!RegisterClassW(&wcChild))
    {
        DWORD error = GetLastError();
        std::wstring errorMessage = L"Failed to register window class. Error code: " + std::to_wstring(error);
        MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
        return NULL;
    }

    // x,y 위치에서 w,h 크기만큼 그려주도록 변환
    int x = rect.left;
    int y = rect.top;
    int width = rect.right + rect.left;
    int height = rect.bottom + rect.top;

    // 자식 윈도우 생성
    HWND hWndChild = CreateWindowExW(
        WS_EX_TOPMOST,
        ws.c_str(),  // 자식 윈도우 클래스 이름
        NULL,                 // 자식 윈도우는 보통 타이틀 없음
        WS_CHILD | WS_VISIBLE | WS_THICKFRAME | WS_TABSTOP, // 자식 스타일
        x, y,              // 위치 (부모 윈도우의 클라이언트 좌표 기준)
        width, height,            // 크기 (너비와 높이)
        hParent,                // 부모 윈도우 핸들
        NULL,                // 메뉴 없음
        hInstance, // 인스턴스 핸들
        data // 추가 데이터
    );

    if (!hWndChild) {
        DWORD error = GetLastError();
        std::wstring errorMessage = L"Failed to create child window. Error code: " + std::to_wstring(error);
        MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
    }

    return hWndChild;
}