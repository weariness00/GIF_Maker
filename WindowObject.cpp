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
	if (child) {
		WindowObject* tempChild = child;
		while (tempChild) {
			tempChild->parent = nullptr;
			WindowObject* nextChild = tempChild->nextSibling;
			delete tempChild;  // �ڽ� ��ü ����
			tempChild = nextChild;
		}
	}

	if (parent)
	{
		if (parent->child == this)
		{
			parent->child = nextSibling;
			if(nextSibling) nextSibling->prevSibling = nullptr;
		}
		else
		{
			nextSibling->prevSibling = prevSibling;
			prevSibling->nextSibling = nextSibling;
		}
	}

	child = nullptr;
	prevSibling = nullptr;
	nextSibling = nullptr;

	objects.erase(find(objects.begin(), objects.end(), this));
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
		while(s->nextSibling)
		{
			s = s->nextSibling;
		}
		s->nextSibling = childObj;
		childObj->prevSibling = s;
		childObj->parent = this;
	}
	else
	{
		childObj->parent = this;
		child = childObj;
	}

	UpdateRootRecursively(childObj, root);

	return true;
}

void WindowObject::Update()
{
}

void WindowObject::UpdateRootRecursively(WindowObject* node, WindowObject* newRoot)
{
	if (!node) return;

	// ���� ����� root�� ����
	node->root = newRoot;

	// ���� �ڽİ� ���� ��忡 ���� ��������� root ����
	UpdateRootRecursively(node->child, newRoot);
	UpdateRootRecursively(node->nextSibling, newRoot);
}

void WindowObject::UpdateWindowTransform(const RECT* worldRect)
{
	if(worldRect)
		wTransform.UpdateWorldTransform(worldRect);
	else
		wTransform.UpdateWorldTransform(nullptr);

	if (nextSibling) nextSibling->UpdateWindowTransform(worldRect);
	if (child) child->UpdateWindowTransform(wTransform.GetWorldRect(false));
}
