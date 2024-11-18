// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
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

#include "TDelegate.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "propsys.lib")

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

inline std::string currentDirPath;
inline const std::string PreviewDirPath = "Preview";

inline char* PWSTRToChar(PWSTR wideString)
{
    // 변환할 버퍼 크기 계산
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, NULL, 0, NULL, NULL);
    if (bufferSize == 0) {
        std::cout << "WideCharToMultiByte 함수 실패" << std::endl;
        return nullptr;
    }

    // 변환된 문자열을 저장할 버퍼
    char* multiByteString = new char[bufferSize];

    // 실제 변환
    int result = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, multiByteString, bufferSize, NULL, NULL);
    if (result == 0) {
        std::cout << "WideCharToMultiByte 함수 실패" << std::endl;
        delete[] multiByteString;
        return nullptr;
    }

    return multiByteString;
}