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

	POINT GetWorldPosition() const;
	POINT GetLocalPosition() const;
	POINT GetSize() const;
	RECT* GetWorldRect(bool isUpdate = true);
	RECT* GetLocalRect() { return &localRect; }

	void SetPosition(const POINT& _position);
	void SetPosition(const LONG x, const LONG y) { SetPosition({ x,y }); }
	void SetPositionX(const LONG x);
	void SetSize(const POINT& _size);
	void SetSize(const LONG x, const LONG y) { SetSize({ x, y }); }

	void SetRect(const RECT& rect);
	void SetRect(const int x, const int y, const int w, const int h) { SetRect({ x,y,w,h }); }
	void SetRect(const int x, const int y, const POINT size) { SetRect({ x,y, size.x, size.y }); }

	void UpdateWorldTransform(const RECT* parentRect);

private:
	WindowObject* ownerObject;
	RECT worldRect;
	RECT localRect;

	POINT position;
	POINT size;
};

