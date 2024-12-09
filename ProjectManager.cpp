#include "ProjectManager.h"

ProjectManager* ProjectManager::Instance;

ProjectManager::ProjectManager()
{
	if (Instance == nullptr)
		Instance = this;
	else
	{
		delete this;
		return;
	}
}

ProjectManager::~ProjectManager()
{
}
