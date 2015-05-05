#include "WindowCreator.h"

#include "WindowWrapper.h"

void WindowCreator::CreateMainWindow(ECSL::World* _world)
{
	WindowWrapper::GetInstance()->OpenWindow(_world);
}

void WindowCreator::UpdateWindow(float _dt)
{
	WindowWrapper::GetInstance()->Update(_dt);
}