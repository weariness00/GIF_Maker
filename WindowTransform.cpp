#include "WindowTransform.h"
#include "WindowObject.h"

WindowTransform::WindowTransform()
{
}

WindowTransform::~WindowTransform()
{
}

void WindowTransform::SetOwnerObject(WindowObject* ownerObj)
{
	ownerObject = ownerObj;
}

RECT* WindowTransform::GetWorldRect(bool isUpdate)
{
	if(isUpdate)
	{
		auto rootObj = ownerObject->GetRoot();
		rootObj->UpdateWindowTransform(nullptr);
	}

	return &worldRect;
}

void WindowTransform::SetRect(const RECT& rect)
{
	localRect = rect;
}

void WindowTransform::UpdateWorldTransform(const RECT* parentRect)
{
	if(parentRect)
	{
		worldRect.left = parentRect->left + localRect.left;
		worldRect.top = parentRect->top + localRect.top;
	}
	else
	{
		worldRect.left = localRect.left;
		worldRect.top = localRect.top;
	}

	worldRect.bottom = localRect.bottom;
	worldRect.right =localRect.right;
}
