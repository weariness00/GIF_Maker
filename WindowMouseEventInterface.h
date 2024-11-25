#pragma once
#include <map>
#include <Windows.h>
#include <type_traits>

class WindowObject;

struct MouseEvent
{
	LONG x, y;
};

class WindowMouseEventInterface
{
public:
	static std::map<HDC, WindowObject*> workEventMap;

public:
	WindowMouseEventInterface();
	virtual ~WindowMouseEventInterface();

	void OnMouseEvent(UINT massage, HDC hdc, WindowObject* winObj, LPARAM lParam);

protected:
	virtual void MouseDownEvent(const MouseEvent& mouseEvent);
	virtual void MouseUpEvent(const MouseEvent& mouseEvent);
	virtual void MousePressEvent(const MouseEvent& mouseEvent);

private:
	bool CheckOnMouse(WindowObject* winObj, const MouseEvent& mouseEvent);

private:
	bool isDragging;
};