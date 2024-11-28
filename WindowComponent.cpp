#include "WindowComponent.h"

WindowComponent::WindowComponent()
{
	isActive = true;
}

WindowComponent::~WindowComponent()
{
}

void WindowComponent::SetActive(bool value)
{
	if(value == true && isActive == false)
	{
		isActive = true;
	}
	else if(value == false && isActive == true)
	{
		isActive = false;
	}

}
