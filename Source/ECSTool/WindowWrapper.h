#pragma once
#ifndef WINDOWWRAPPER_H
#define WINDOWWRAPPER_H

#include "MainWindow.h"
#include "ECSL/Framework/World.h"

public class WindowWrapper
{
public:
	static WindowWrapper* GetInstance();
	~WindowWrapper();

	void OpenWindow(ECSL::World* _world);
	void ChangeWorld(ECSL::World* _world);
	void Update(float _dt);

private:
	WindowWrapper();
	void InitializeWindow(ECSL::World* _world);

private:
	bool m_isActive;

	gcroot<ECSTool::MainWindow^> m_window;
};

#endif