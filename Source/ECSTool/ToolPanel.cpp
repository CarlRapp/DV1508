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

	this->toolPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;

	this->toolPanel->BackColor = System::Drawing::Color::WhiteSmoke;
	this->toolPanel->Location = System::Drawing::Point(3, 3);
	this->toolPanel->Name = L"ToolPanel";
	//this->toolPanel->AutoSize = true;
	//this->toolPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	this->toolPanel->Size = System::Drawing::Size(627, 23);
	this->toolPanel->TabIndex = 0;

	this->togglePickingButton = (gcnew System::Windows::Forms::CheckBox());
	this->togglePickingButton->Name = L"PickingButton";
	this->togglePickingButton->Text = L"3D Picking";
	this->togglePickingButton->Size = System::Drawing::Size(90, 20);
	this->togglePickingButton->Location = System::Drawing::Point(50, 1);
	this->togglePickingButton->Click += gcnew System::EventHandler(this, &MainWindow::TogglePicking_Clicked);
	this->togglePickingButton->Checked = true;
	this->toolPanel->Controls->Add(this->togglePickingButton);

	this->togglePauseButton = (gcnew System::Windows::Forms::CheckBox());
	this->togglePauseButton->Name = L"PickingButton";
	this->togglePauseButton->Text = L"Pause";
	this->togglePauseButton->Size = System::Drawing::Size(60, 20);
	this->togglePauseButton->Location = System::Drawing::Point(togglePickingButton->Location.X + togglePickingButton->Size.Width, 1);
	this->togglePauseButton->Click += gcnew System::EventHandler(this, &MainWindow::TogglePause_Clicked);
	this->toolPanel->Controls->Add(this->togglePauseButton);

	this->toggleOneTickUpdate = (gcnew System::Windows::Forms::Button());
	this->toggleOneTickUpdate->Name = L"PickingButton";
	this->toggleOneTickUpdate->Text = L"Update";
	this->toggleOneTickUpdate->Size = System::Drawing::Size(55, 19);
	this->toggleOneTickUpdate->Location = System::Drawing::Point(togglePauseButton->Location.X + togglePauseButton->Size.Width, 1);
	this->toggleOneTickUpdate->Click += gcnew System::EventHandler(this, &MainWindow::ButtonOneTickUpdate_Clicked);
	this->toggleOneTickUpdate->Font = (gcnew System::Drawing::Font("Microsoft Sans Serif", 8.0f, System::Drawing::FontStyle::Regular));
	this->toolPanel->Controls->Add(this->toggleOneTickUpdate);

	this->toolLabel = (gcnew System::Windows::Forms::Label());
	this->toolPanel->Controls->Add(this->toolLabel);
	this->toolLabel->Text = L"Tools:";
	this->toolLabel->Location = System::Drawing::Point(0, 1);
	this->toolLabel->Font = (gcnew System::Drawing::Font("Microsoft Sans Serif", 10.0f, System::Drawing::FontStyle::Bold));

	this->Controls->Add(this->toolPanel);

	this->dataPanel->ResumeLayout(true);
	this->ResumeLayout(true);
}

void MainWindow::TogglePicking_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	m_graphics->TogglePicking();
}

void MainWindow::TogglePause_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	toggledPause = true;
	this->togglePauseButton->Checked = paused;
	//PAUSE GAME
}

void MainWindow::ButtonOneTickUpdate_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	oneTickUpdate = true;
}
