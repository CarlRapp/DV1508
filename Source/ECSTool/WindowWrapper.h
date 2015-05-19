#pragma once
#ifndef WINDOWWRAPPER_H
#define WINDOWWRAPPER_H

#include "MainWindow.h"
#include "ECSL/Framework/World.h"
//#include "Renderer/GraphicsHigh.h"

public class WindowWrapper
{
public:
	static WindowWrapper* GetInstance();
	~WindowWrapper();

	void OpenWindow(ECSL::World* _world, Renderer::GraphicsHigh* _graphics);
	void ChangeWorld(ECSL::World* _world, Renderer::GraphicsHigh* _graphics);
	void Update(float _dt);
	void PickingOccured();

private:
	WindowWrapper();
	void InitializeWindow(ECSL::World* _world, Renderer::GraphicsHigh* _graphics);
private:
	bool m_isActive;

	gcroot<ECSTool::MainWindow^> m_window;
};

#endif