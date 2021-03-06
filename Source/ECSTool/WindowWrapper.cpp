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
	ChangeWorld(_world,_graphics);
}

void WindowWrapper::ChangeWorld(ECSL::World* _world, Renderer::GraphicsHigh* _graphics)
{
	if (!m_isActive)
		return;
	m_window->SetWorld(_world);
	m_window->SetGraphics(_graphics);
}

void WindowWrapper::Update(float _dt, bool _paused)
{
	if (m_isActive)
		m_window->Update(_dt, _paused);
}

bool WindowWrapper::HasToggledPause()
{
	if (m_isActive)
		return m_window->HasToggledPause();
	return false;
}

bool WindowWrapper::OneTickUpdate()
{
	if (m_isActive)
		return m_window->OneTickUpdate();
	return false;
}

void WindowWrapper::PickingOccured()
{
	if (m_isActive)
		m_window->PickingOccured();
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