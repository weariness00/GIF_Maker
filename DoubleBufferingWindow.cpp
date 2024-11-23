#include "DoubleBufferingWindow.h"

DoubleBufferingWindow::DoubleBufferingWindow(HWND hwnd)
    : m_hwnd(hwnd), m_hdcMem(NULL), m_hBitmap(NULL) {
    // 초기화 시 메모리 DC와 비트맵을 생성
    m_hdc = GetDC(hwnd);
    m_hdcMem = CreateCompatibleDC(m_hdc);

    RECT rect;
    GetClientRect(hwnd, &rect);
    m_hBitmap = CreateCompatibleBitmap(m_hdc, rect.right, rect.bottom);
    SelectObject(m_hdcMem, m_hBitmap);
}

DoubleBufferingWindow::~DoubleBufferingWindow()
{
    // 자원 해제
    DeleteObject(m_hBitmap);
    DeleteDC(m_hdcMem);
    ReleaseDC(m_hwnd, m_hdc);
}

void DoubleBufferingWindow::OnPaint(const std::function<void(HDC)>& paintFunc) const
{
    HBITMAP oldBitmap = (HBITMAP)SelectObject(m_hdcMem, m_hBitmap);

    // 메모리 DC에 그리기
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    FillRect(m_hdcMem, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

	// GDI+로 이미지 그리기 (여기서 실제 그림을 그립니다)
    paintFunc(m_hdcMem);

    // 메모리 DC에 그려진 내용을 화면에 복사
    BitBlt(m_hdc, 0, 0, rect.right, rect.bottom, m_hdcMem, 0, 0, SRCCOPY);

	// 복구: 이전 비트맵을 다시 선택
    SelectObject(m_hdcMem, oldBitmap);

    // 기존 비트맵 해제
    DeleteObject(m_hBitmap);
}