#include "DoubleBufferingWindow.h"

DoubleBufferingWindow::DoubleBufferingWindow(HWND hwnd)
    : m_hwnd(hwnd), m_hdcMem(NULL), m_hBitmap(NULL) {
    // �ʱ�ȭ �� �޸� DC�� ��Ʈ���� ����
    m_hdc = GetDC(hwnd);
    m_hdcMem = CreateCompatibleDC(m_hdc);

    RECT rect;
    GetClientRect(hwnd, &rect);
    m_hBitmap = CreateCompatibleBitmap(m_hdc, rect.right, rect.bottom);
    SelectObject(m_hdcMem, m_hBitmap);
}

DoubleBufferingWindow::~DoubleBufferingWindow()
{
    // �ڿ� ����
    DeleteObject(m_hBitmap);
    DeleteDC(m_hdcMem);
    ReleaseDC(m_hwnd, m_hdc);
}

void DoubleBufferingWindow::OnPaint(const std::function<void(HDC)>& paintFunc) const
{
    HBITMAP oldBitmap = (HBITMAP)SelectObject(m_hdcMem, m_hBitmap);

    // �޸� DC�� �׸���
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    FillRect(m_hdcMem, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

	// GDI+�� �̹��� �׸��� (���⼭ ���� �׸��� �׸��ϴ�)
    paintFunc(m_hdcMem);

    // �޸� DC�� �׷��� ������ ȭ�鿡 ����
    BitBlt(m_hdc, 0, 0, rect.right, rect.bottom, m_hdcMem, 0, 0, SRCCOPY);

	// ����: ���� ��Ʈ���� �ٽ� ����
    SelectObject(m_hdcMem, oldBitmap);

    // ���� ��Ʈ�� ����
    DeleteObject(m_hBitmap);
}