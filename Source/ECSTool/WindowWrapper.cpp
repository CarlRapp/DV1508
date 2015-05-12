#include "WindowWrapper.h"

WindowWrapper* WindowWrapper::GetInstance()
{
	static WindowWrapper* instance = new WindowWrapper();
	return instance;
}

WindowWrapper::WindowWrapper()
{
	m_isActive = false;
}

WindowWrapper::~WindowWrapper()
{

}

void WindowWrapper::OpenWindow(ECSL::World* _world, Renderer::GraphicsHigh* _graphics)
{
	InitializeWindow(_world, _graphics);
	ChangeWorld(_world);
}

void WindowWrapper::ChangeWorld(ECSL::World* _world)
{
	if (!m_isActive)
		return;
	m_window->SetWorld(_world);
}

void WindowWrapper::Update(float _dt)
{
	if (m_isActive)
		m_window->Update(_dt);
}

void WindowWrapper::InitializeWindow(ECSL::World* _world, Renderer::GraphicsHigh* _graphics)
{
	if (m_isActive)
		return;

	m_window = gcnew ECSTool::MainWindow();
	m_window->SetWorld(_world);
	m_window->SetGraphics(_graphics);

	m_window->InitializeTool();
	m_window->Show();
	m_isActive = true;
}