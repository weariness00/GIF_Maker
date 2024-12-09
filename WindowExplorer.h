#pragma once
#include "TDelegate.h"
#include <windows.h>
#include <shlobj.h>

class WindowExplorer
{
public:
	WindowExplorer();
	~WindowExplorer();

public:
	void CloseExplorerWindows();

	HRESULT InitVariantFromObject(IUnknown* punk, VARIANT* pvar);
	HRESULT ParseItemAsVariant(PCWSTR pszItem, IBindCtx* pbc, VARIANT* pvar);
	HRESULT GetKnownFolderAsVariant(REFKNOWNFOLDERID kfid, VARIANT* pvar);
	HRESULT GetShellItemFromCommandLine(REFIID riid, void** ppv);
	HRESULT GetShellItemFromCommandLineAsVariant(VARIANT* pvar);
	void OpenWindow();

	// 파일 열기 대화 상자
	void FileOpenDialog(HWND hWnd);
	PWSTR GetFilePath() const { return pszFilePath; }

public:
	TDelegate successFileOpenEvent; // PWSTR자료형만 받는다.

private:
	PWSTR fileOpenDialogPath;
	PWSTR pszFilePath;
};