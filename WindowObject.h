#pragma once
#include <string>
#include "WindowTransform.h"
#include "WindowComponent.h"
#include <vector>

class WindowObject : public WindowComponent
{
public:
	static int ObjectID;
	static std::vector<WindowObject*> objects;

public:
	WindowObject();
	virtual ~WindowObject();

	WindowObject* GetRoot() const { return root; }
	WindowObject* GetParent() const { return parent; }
	bool SetParent(WindowObject* parentObj);
	bool SetChild(WindowObject* childObj);

	void UpdateWindowTransform(const RECT* worldRect);
	virtual void Update();

private:
	void UpdateRootRecursively(WindowObject* node, WindowObject* newRoot);

public:
	int id;
	std::string name;
	WindowTransform wTransform;

private:
	WindowObject* root;
	WindowObject* parent;
	WindowObject* prevSibling;
	WindowObject* nextSibling;
	WindowObject* child;
};