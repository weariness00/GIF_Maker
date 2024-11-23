#pragma once
#include <Windows.h>

class WindowObject;

class WindowTransform
{
public:
	WindowTransform();
	~WindowTransform();

public:
	void SetOwnerObject(WindowObject* ownerObj);

	RECT* GetWorldRect(bool isUpdate = true);
	RECT* GetLocalRect() { return &localRect; }
	void SetRect(const RECT& rect);
	void SetRect(const int x, const int y, const int w, const int h) { SetRect({ x,y,w,h }); }

	void UpdateWorldTransform(const RECT* parentRect);

private:
	WindowObject* ownerObject;
	RECT worldRect;
	RECT localRect;
};

