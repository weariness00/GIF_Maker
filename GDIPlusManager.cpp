#include "GDIPlusManager.h"

GDIPlusManager* GDIPlusManager::Instance = nullptr;

GDIPlusManager::GDIPlusManager()
{
	if (Instance == nullptr)
		Instance = this;
	else
		delete this;
}

GDIPlusManager::~GDIPlusManager()
{
	for (std::pair<HDC__* const, Graphics*> p : graphics)
	{
		delete p.second;
	}

	graphics.clear();
}

void GDIPlusManager::CreateGraphics(HDC hdc)
{
	if (!graphics.contains(hdc))
	{
		graphics[hdc] = new Graphics(hdc);
	}
}

Graphics* GDIPlusManager::GetGraphics(HDC hdc)
{
	if (graphics.contains(hdc))
		return graphics[hdc];
	return nullptr;
}

void GDIPlusManager::ReleaseGraphics(HDC hdc)
{
	if (graphics.contains(hdc))
	{
		delete graphics[hdc];
		graphics.erase(hdc);
	}
}
