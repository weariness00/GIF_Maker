#include "WindowMouseEventInterface.h"
#include "WindowObject.h"

WindowMouseEventInterface::WindowMouseEventInterface()
{
    isDragging = false;
}

WindowMouseEventInterface::~WindowMouseEventInterface()
{
}

void WindowMouseEventInterface::OnMouseEvent(UINT massage, WindowObject* winObj, LPARAM lParam)
{
    MouseEvent mouseEvent;
    mouseEvent.x = LOWORD(lParam);
    mouseEvent.y = HIWORD(lParam);

    bool isOnMouse;
    switch (massage)
    {
    case WM_LBUTTONDOWN:
        isOnMouse = CheckOnMouse(winObj, mouseEvent);
        if (isOnMouse == true && isDragging == false)
        {
            isDragging = true;
            MouseDownEvent(winObj, mouseEvent);
        }
        break;
    case WM_MOUSEMOVE:
        if (isDragging)
        {
            MousePressEvent(winObj, mouseEvent);
        }
        break;
    case WM_LBUTTONUP:
        isOnMouse = CheckOnMouse(winObj, mouseEvent);
        if (isOnMouse == false && isDragging == true)
        {
            MouseUpEvent(winObj, mouseEvent);
        }
        isDragging = false;
        break;
    }
}

void WindowMouseEventInterface::MouseDownEvent(WindowObject* winObj, const MouseEvent& mouseEvent)
{
}

void WindowMouseEventInterface::MouseUpEvent(WindowObject* winObj, const MouseEvent& mouseEvent)
{
}

void WindowMouseEventInterface::MousePressEvent(WindowObject* winObj, const MouseEvent& mouseEvent)
{

}

bool WindowMouseEventInterface::CheckOnMouse(WindowObject* winObj, const MouseEvent& mouseEvent)
{
    WindowTransform* wTransform = &winObj->wTransform;
    auto position = wTransform->GetPosition();
    auto size = wTransform->GetSize();

    return mouseEvent.x >= position.x && mouseEvent.x <= position.x + size.x &&
        mouseEvent.y >= position.y && mouseEvent.y <= position.y + size.y;
}
