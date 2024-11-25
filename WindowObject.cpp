#include "WindowObject.h"

std::vector<WindowObject*> WindowObject::objects;

WindowObject::WindowObject()
{
	root = this;
	wTransform.SetOwnerObject(this);


	objects.emplace_back(this);
}

WindowObject::~WindowObject()
{
}

bool WindowObject::SetParent(WindowObject* parentObj)
{

	return true;
}

bool WindowObject::SetChild(WindowObject* childObj)
{
	childObj->root = root;
	if(child != nullptr)
	{
		WindowObject* s = child;
		while(s->sibling)
		{
			s = s->sibling;
		}
		s->sibling = childObj;
	}
	else
	{
		child = childObj;
	}
	return true;
}

void WindowObject::Update()
{
}

void WindowObject::UpdateWindowTransform(const RECT* worldRect)
{
	if(worldRect)
		wTransform.UpdateWorldTransform(worldRect);
	else
		wTransform.UpdateWorldTransform(nullptr);

	if (sibling) sibling->UpdateWindowTransform(worldRect);
	if (child) child->UpdateWindowTransform(wTransform.GetWorldRect(false));
}
