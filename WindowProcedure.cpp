#include "WindowProcedure.h"
#include <exception>

LRESULT WindowProcedure::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WindowProcedure* pThis = NULL;

    if (uMsg == WM_NCCREATE || uMsg == WM_CREATE || uMsg == WM_INITDIALOG) {
        // WM_NCCREATE에서 lParam을 통해 this 포인터 설정
        LPCREATESTRUCT pCreate = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<WindowProcedure*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else {
        // 이후 메시지에서는 GWLP_USERDATA에서 this 포인터를 가져옴
        pThis = reinterpret_cast<WindowProcedure*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis) {
        try {
            // HandleMessage 호출
            return pThis->HandleMessage(hwnd, uMsg, wParam, lParam);
        }
        catch (const std::exception& e) {
            // 예외 처리 (디버깅을 위해 메시지 출력)
            MessageBoxA(hwnd, e.what(), "Error in HandleMessage", MB_OK | MB_ICONERROR);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

WindowProcedure::WindowProcedure()
{
}
