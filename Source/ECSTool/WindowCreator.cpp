#include "WindowCreator.h"

#include "MainWindow.h"

void WindowCreator::CreateMainWindow(ECSL::World* _world)
{
	ECSTool::MainWindow^ mainWindow = gcnew ECSTool::MainWindow();
	mainWindow->Show();
}
