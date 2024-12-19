#include "WindowMouseEventInterface.h"
#include "WindowObject.h"

std::map<HDC, WindowObject*> WindowMouseEventInterface::workEventMap;

WindowMouseEventInterface::WindowMouseEventInterface()
{
    isDragging = false;
}

WindowMouseEventInterface::~WindowMouseEventInterface()
{
}

void WindowMouseEventInterface::OnMouseEvent(UINT massage, HDC hdc, WindowObject* winObj, LPARAM lParam)
{
    MouseEvent mouseEvent;
    mouseEvent.x = LOWORD(lParam);
    mouseEvent.y = HIWORD(lParam);
    mouseEvent.moveX = mouseEvent.x - prevEvent.x;
    mouseEvent.moveY = mouseEvent.y - prevEvent.y;

    bool isOnMouse;
    switch (massage)
    {
    case WM_LBUTTONDOWN:
        if (workEventMap[hdc] != nullptr) break;
        if (CheckOnMouse(winObj, mouseEvent))
        {
            workEventMap[hdc] = winObj;
            isDragging = true;
            MouseDownEvent(mouseEvent);
        }
        break;
    case WM_MOUSEMOVE:
        if (workEventMap[hdc] != winObj) break;
        if (isDragging)
        {
            MousePressEvent(mouseEvent);
        }
        break;
    case WM_LBUTTONUP:
        if (workEventMap[hdc] != winObj) break;
        workEventMap[hdc] = nullptr;
        if (!CheckOnMouse(winObj, mouseEvent))
        {
            MouseUpEvent(mouseEvent);
        }
        isDragging = false;
        break;
    }

    prevEvent = mouseEvent;
}

void WindowMouseEventInterface::MouseDownEvent(const MouseEvent& mouseEvent)
{
}

void WindowMouseEventInterface::MouseUpEvent(const MouseEvent& mouseEvent)
{
}

void WindowMouseEventInterface::MousePressEvent(const MouseEvent& mouseEvent)
{

}

bool WindowMouseEventInterface::CheckOnMouse(WindowObject* winObj, const MouseEvent& mouseEvent)
{
    WindowTransform* wTransform = &winObj->wTransform;
    auto position = wTransform->GetWorldPosition();
    auto size = wTransform->GetSize();

    return mouseEvent.x >= position.x && mouseEvent.x <= position.x + size.x &&
        mouseEvent.y >= position.y && mouseEvent.y <= position.y + size.y;
}
