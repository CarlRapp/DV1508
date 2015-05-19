#include "WindowCreator.h"
#include "WindowWrapper.h"
#include "Renderer/GraphicsHigh.h"


void WindowCreator::CreateMainWindow(ECSL::World* _world, Renderer::GraphicDevice* _graphics)
{
	WindowWrapper::GetInstance()->OpenWindow(_world,static_cast<Renderer::GraphicsHigh*>(_graphics));
}

void WindowCreator::UpdateWindow(float _dt)
{
	WindowWrapper::GetInstance()->Update(_dt);
}