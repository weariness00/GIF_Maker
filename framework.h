// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
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

#include "TDelegate.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "propsys.lib")

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

inline std::string currentDirPath;
inline std::string PreviewDirPath = "Preview";

inline PWSTR StringToPWSTR(const std::string& str) {
    // 입력된 string을 const char*로 가져오기
    const char* cstr = str.c_str();

    // 필요한 유니코드 문자열 크기 계산
    int sizeRequired = MultiByteToWideChar(CP_ACP, 0, cstr, -1, nullptr, 0);
    if (sizeRequired <= 0) {
        return nullptr;  // 변환 실패 시 nullptr 반환
    }

    // PWSTR을 위한 메모리 할당
    PWSTR pwstr = new wchar_t[sizeRequired];

    // 실제 변환: ANSI(멀티바이트)를 유니코드로 변환
    MultiByteToWideChar(CP_ACP, 0, cstr, -1, pwstr, sizeRequired);

    return pwstr;
}

inline std::string PWSTRToString(PWSTR pwsz) {
    // Convert PWSTR (UTF-16) to std::string (UTF-8)
    int requiredSize = WideCharToMultiByte(CP_UTF8, 0, pwsz, -1, NULL, 0, NULL, NULL);
    if (requiredSize > 0) {
        // Allocate memory for UTF-8 string
        char* utf8String = new char[requiredSize];
        WideCharToMultiByte(CP_UTF8, 0, pwsz, -1, utf8String, requiredSize, NULL, NULL);

        // Create std::string from UTF-8 char array
        std::string result(utf8String);

        delete[] utf8String;  // Free memory
        return result;
    }
    return std::string();  // Return empty string if conversion fails
}

// 문자열을 따음표로 감싸기
// www\dddds\d d.gif -> "www\dddds\d d.gif"
inline std::string WrapingQuotes(const std::string s)
{
    return "\"" + s + "\"";
}