#include "WindowObject.h"
#include <typeinfo>

int WindowObject::ObjectID = 0;
std::vector<WindowObject*> WindowObject::objects;

WindowObject::WindowObject()
{
	id = ObjectID++;
	name = typeid(this).name();

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
		childObj->parent = this;
	}
	else
	{
		childObj->parent = this;
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
