#include "MainWindow.h"
#include "ECSL/Managers/ComponentTypeManager.h"
#include <SDL/SDL.h>
#include <msclr\marshal_cppstd.h>
using namespace ECSTool;


#pragma region Component Panel section

#pragma region Create Component Panel
void MainWindow::CreateComponentPanel()
{
	this->SuspendLayout();

	//	Create the panel object
	this->componentPanel = (gcnew System::Windows::Forms::Panel());
	this->componentPanel->SuspendLayout();

	//this->componentPanel->BackColor = System::Drawing::Color::Green;
	this->componentPanel->Location = System::Drawing::Point(this->entityPanel->Size.Width + 13, 13);
	this->componentPanel->Name = L"ComponentPanel";
	this->componentPanel->AutoSize = true;
	this->componentPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	this->componentPanel->TabIndex = 0;

	//	Create the listbox object
	this->componentPanel_ComponentList = (gcnew System::Windows::Forms::ListBox());
	this->componentPanel_ComponentList->FormattingEnabled = true;
	this->componentPanel_ComponentList->Location = System::Drawing::Point(4, 4);
	this->componentPanel_ComponentList->Name = L"ComponentListBox";
	this->componentPanel_ComponentList->Size = this->entityPanel_EntityList->Size;
	this->componentPanel_ComponentList->TabIndex = 0;
	this->componentPanel_ComponentList->SelectedIndexChanged += gcnew System::EventHandler(this, &MainWindow::componentPanel_ComponentList_SelectedIndexChanged);

	//	Create the remove component button
	this->componentPanel_RemoveComponentButton = (gcnew System::Windows::Forms::Button());
	this->componentPanel_RemoveComponentButton->Name = L"RemoveComponentButton";
	this->componentPanel_RemoveComponentButton->Text = L"Remove";
	this->componentPanel_RemoveComponentButton->Size = System::Drawing::Size(55, 20);
	this->componentPanel_RemoveComponentButton->Location = System::Drawing::Point(this->componentPanel_ComponentList->Location.X + this->componentPanel_ComponentList->Size.Width - this->componentPanel_RemoveComponentButton->Size.Width, this->componentPanel_ComponentList->Size.Height + 8);

	this->componentPanel_RemoveComponentButton->Click += gcnew System::EventHandler(this, &MainWindow::RemoveComponent);

	//	Create the add component button
	this->componentPanel_AddComponentButton = (gcnew System::Windows::Forms::Button());
	this->componentPanel_AddComponentButton->Name = L"AddComponent";
	this->componentPanel_AddComponentButton->Text = L"Add";
	this->componentPanel_AddComponentButton->Size = System::Drawing::Size(55, 20);
	this->componentPanel_AddComponentButton->Location = System::Drawing::Point(this->componentPanel_ComponentList->Location.X, this->componentPanel_ComponentList->Size.Height + 8);

	this->componentPanel_AddComponentButton->Click += gcnew System::EventHandler(this, &MainWindow::componentPanel_AddComponent_Clicked);

	//	Hook up
	this->componentPanel->Controls->Add(this->componentPanel_ComponentList);
	this->componentPanel->Controls->Add(this->componentPanel_RemoveComponentButton);
	this->componentPanel->Controls->Add(this->componentPanel_AddComponentButton);

	this->Controls->Add(this->componentPanel);

	this->componentPanel->ResumeLayout(true);
	this->ResumeLayout(true);
}
#pragma endregion

std::string MainWindow::GetComponentName(unsigned int _cId)
{
	std::string sComponent = ECSL::ComponentTypeManager::GetInstance().GetComponentType(_cId)->GetName();

	return sComponent;
}

void MainWindow::UpdateComponentPanelList(int _entityId)
{
	this->componentPanel_ComponentList->BeginUpdate();
	this->componentPanel_ComponentList->Items->Clear();
	if (_entityId == -1)
	{
		ClearSelectedComponent();
		UpdateDataPanelList(m_currentEntity, m_currentComponent);
		this->componentPanel_ComponentList->EndUpdate();
		return;
	}


	
	
	std::vector<unsigned int> components;
	m_world->GetEntityComponents(components, _entityId);
	bool componentFound = false;
	int index = -1;
	for (int n = 0; n < components.size(); ++n)
	{
		int tempIndex = this->componentPanel_ComponentList->Items->Add(gcnew System::String(GetComponentName(components.at(n)).c_str()));

		if (components.at(n) == m_currentComponent)
		{
			componentFound = true;
			index = tempIndex;
		}

	}

	//	If there is a entity selected, select it afterwards (if it wasnt found, set current to -1)
	if (componentFound)
		this->componentPanel_ComponentList->SetSelected(index, true);
	else
		ClearSelectedComponent();

	UpdateDataPanelList(m_currentEntity, m_currentComponent);
	this->componentPanel_ComponentList->EndUpdate();
	
}

#pragma region Selected Index
System::Void MainWindow::componentPanel_ComponentList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
{
	std::string component = toString(componentPanel_ComponentList->SelectedItem);
	m_currentComponent = ECSL::ComponentTypeManager::GetInstance().GetTableId(component);
	UpdateDataPanelList(m_currentEntity, m_currentComponent);
}
#pragma endregion


#pragma region Remove Component
void MainWindow::RemoveComponent(System::Object^ sender, System::EventArgs^ e)
{
	if (m_currentComponent == -1)
		return;

	std::string component = toString(componentPanel_ComponentList->SelectedItem);
	m_world->RemoveComponentFrom(component, m_currentEntity);



	ClearSelectedComponent();
	m_forceUpdate = true;
}
#pragma endregion

void MainWindow::ClearSelectedComponent()
{
	m_currentComponent = -1;
}

#pragma endregion

System::Void MainWindow::componentPanel_AddComponent_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	if (m_currentEntity == -1)
		return;

	if (this->addComponentPanel->Visible)
		this->addComponentPanel->Hide();
	else
	{
		PopulateComponentSubPanel(m_currentEntity);
		this->addComponentPanel->Show();
	}

}
