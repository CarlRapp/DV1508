#include "MainWindow.h"
#include "ECSL/Managers/ComponentTypeManager.h"
#include <SDL/SDL.h>
#include <msclr\marshal_cppstd.h>
#include <algorithm>
using namespace ECSTool;

void MainWindow::CreateToolPanel()
{
	this->SuspendLayout();

	//	Create the panel object
	this->toolPanel = (gcnew System::Windows::Forms::Panel());
	this->toolPanel->SuspendLayout();

	//this->dataPanel->BackColor = System::Drawing::Color::Yellow;
	this->toolPanel->Location = System::Drawing::Point(this->dataPanel->Location.X + this->dataPanel->Size.Width, this->entityPanel->Location.Y);
	this->toolPanel->Name = L"ToolPanel";
	this->toolPanel->AutoSize = true;
	this->toolPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	this->toolPanel->TabIndex = 0;

	this->togglePickingButton = (gcnew System::Windows::Forms::CheckBox());
	this->togglePickingButton->Name = L"PickingButton";
	this->togglePickingButton->Text = L"3D Picking";
	this->togglePickingButton->Size = System::Drawing::Size(130, 20);
	this->togglePickingButton->Location = System::Drawing::Point(this->entityPanel_EntityList->Location.X, this->entityPanel_EntityList->Location.Y);
	this->togglePickingButton->Click += gcnew System::EventHandler(this, &MainWindow::TogglePicking_Clicked);
	this->toolPanel->Controls->Add(this->togglePickingButton);

	this->togglePauseButton = (gcnew System::Windows::Forms::CheckBox());
	this->togglePauseButton->Name = L"PickingButton";
	this->togglePauseButton->Text = L"Pause";
	this->togglePauseButton->Size = System::Drawing::Size(130, 20);
	this->togglePauseButton->Location = System::Drawing::Point(this->entityPanel_EntityList->Location.X, this->entityPanel_EntityList->Location.Y + this->togglePickingButton->Size.Height);
	this->togglePauseButton->Click += gcnew System::EventHandler(this, &MainWindow::TogglePause_Clicked);
	this->toolPanel->Controls->Add(this->togglePauseButton);

	this->toolLabel = (gcnew System::Windows::Forms::Label());
	this->toolPanel->Controls->Add(this->toolLabel);
	this->toolLabel->Text = L"Tools";
	this->toolLabel->Location = System::Drawing::Point(0, 0);
	this->toolLabel->Font = (gcnew System::Drawing::Font("Microsoft Sans Serif", 10.0f, System::Drawing::FontStyle::Bold));

	this->Controls->Add(this->toolPanel);

	this->dataPanel->ResumeLayout(true);
	this->ResumeLayout(true);

	m_graphics->TogglePicking();
}

void MainWindow::TogglePicking_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	m_graphics->TogglePicking();
}

void MainWindow::TogglePause_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	//PAUSE GAME
}

