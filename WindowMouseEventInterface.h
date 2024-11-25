#pragma once
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
	WindowMouseEventInterface();
	virtual ~WindowMouseEventInterface();

	void OnMouseEvent(UINT massage, WindowObject* winObj, LPARAM lParam);

protected:
	virtual void MouseDownEvent(WindowObject* winObj, const MouseEvent& mouseEvent);
	virtual void MouseUpEvent(WindowObject* winObj, const MouseEvent& mouseEvent);
	virtual void MousePressEvent(WindowObject* winObj, const MouseEvent& mouseEvent);

private:
	bool CheckOnMouse(WindowObject* winObj, const MouseEvent& mouseEvent);

private:
	bool isDragging;
};