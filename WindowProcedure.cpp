#include "WindowProcedure.h"
#include <exception>

LRESULT WindowProcedure::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WindowProcedure* pThis = NULL;

    if (uMsg == WM_NCCREATE || uMsg == WM_CREATE || uMsg == WM_INITDIALOG) {
        // WM_NCCREATE���� lParam�� ���� this ������ ����
        LPCREATESTRUCT pCreate = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<WindowProcedure*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else {
        // ���� �޽��������� GWLP_USERDATA���� this �����͸� ������
        pThis = reinterpret_cast<WindowProcedure*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis) {
        try {
            // HandleMessage ȣ��
            return pThis->HandleMessage(hwnd, uMsg, wParam, lParam);
        }
        catch (const std::exception& e) {
            // ���� ó�� (������� ���� �޽��� ���)
            MessageBoxA(hwnd, e.what(), "Error in HandleMessage", MB_OK | MB_ICONERROR);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

WindowProcedure::WindowProcedure()
{
}
