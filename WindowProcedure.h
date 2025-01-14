#pragma once
#include <windows.h>
#include <windowsx.h>

class WindowProcedure
{
protected:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	WindowProcedure();
	virtual ~WindowProcedure() {}

public:
	virtual void Instanciate(HINSTANCE hInst, HWND hwnd) = 0;
	virtual LRESULT HandleMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }
	void Test() {
		int a = 10;
		int b = 20;
		a = b;
	}
};

