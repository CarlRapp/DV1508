#include "WindowCreator.h"
#include "WindowWrapper.h"
#include "Renderer/GraphicsHigh.h"


void WindowCreator::CreateMainWindow(ECSL::World* _world, Renderer::GraphicDevice* _graphics)
{
	WindowWrapper::GetInstance()->OpenWindow(_world,static_cast<Renderer::GraphicsHigh*>(_graphics));
}

void WindowCreator::UpdateWindow(float _dt, bool paused)
{
	WindowWrapper::GetInstance()->Update(_dt, paused);
}

bool WindowCreator::HasToggledPause()
{
	return WindowWrapper::GetInstance()->HasToggledPause();
}

void WindowCreator::PickingOccured()
{
	WindowWrapper::GetInstance()->PickingOccured();
}