#pragma once
class WindowComponent
{
public:
	WindowComponent();
	~WindowComponent();

public:
	void SetActive(bool value);
	bool GetActive() const { return isActive; }

private:
	bool isActive;
};

