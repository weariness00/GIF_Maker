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

POINT WindowTransform::GetWorldPosition() const
{
	auto rootObj = ownerObject->GetRoot();
	rootObj->UpdateWindowTransform(nullptr);

	return { worldRect.left, worldRect.top };
}

POINT WindowTransform::GetLocalPosition() const
{
	return position;
}

POINT WindowTransform::GetSize() const
{
	return size;
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

void WindowTransform::SetPosition(const POINT& _position)
{
	position = _position;
	localRect.left = _position.x;
	localRect.top = _position.y;
}

void WindowTransform::SetSize(const POINT& _size)
{
	size = _size;
	localRect.right = size.x;
	localRect.bottom = size.y;
}

void WindowTransform::SetRect(const RECT& rect)
{
	localRect = rect;
	position.x = rect.left;
	position.y = rect.top;
	size.x = rect.right;
	size.y = rect.bottom;
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
