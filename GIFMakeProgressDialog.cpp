#include "GIFMakeProgressDialog.h"
#include "framework.h"

GIFMakeProgressDialog::GIFMakeProgressDialog()
{
}

void GIFMakeProgressDialog::Instanciate(HINSTANCE hInst, HWND hwnd){
	int result = DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_GIFMakeProgress), hwnd, WindowProc, reinterpret_cast<LPARAM>(this));
    if (result == -1) ErrorDialog();
}

LRESULT GIFMakeProgressDialog::HandleMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        ShowWindow(hDlg, SW_SHOW);
        return TRUE;

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {

        }
        break;
    }
    case WM_DESTROY:
        isInstanciate = false;
        break;
    }
    return FALSE;
}
