#pragma once
#include "MainView.h"
#include "WindowProcedure.h"

class GIFMakeProgressDialog : public WindowProcedure
{
public:
	GIFMakeProgressDialog();
	virtual ~GIFMakeProgressDialog() {};

public:
	void Instanciate(HINSTANCE hInst, HWND hwnd) override;
	virtual LRESULT HandleMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) override;

public:
	bool isInstanciate = false;
};

