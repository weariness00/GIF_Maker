#pragma once
#include "WindowExplorer.h"
#include "GDIPlusManager.h"

class ProjectManager
{
public:
	static ProjectManager* Instance;

	ProjectManager();
	~ProjectManager();

public:
	WindowExplorer videoOpenExploprer;
	GDIPlusManager gdiPlusManager;
};

