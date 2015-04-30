#include "MainWindow.h"
#include "ECSL/Managers/ComponentTypeManager.h"
#include <SDL/SDL.h>
#include <msclr\marshal_cppstd.h>


using namespace ECSTool;

void MainWindow::SetWorld(ECSL::World* _world)
{
	m_world	=	_world;
	currentEntity = 0;
}

#pragma region Entity Panel section


#pragma region Create Entity Panel
void MainWindow::CreateEntityPanel()
{
	//	Create the panel object
	this->entityPanel = (gcnew System::Windows::Forms::Panel());
	this->entityPanel->SuspendLayout();

	this->entityPanel->Location = System::Drawing::Point(13, 13);
	this->entityPanel->Name = L"EntityPanel";
	this->entityPanel->Size = System::Drawing::Size(200, 335);
	this->entityPanel->TabIndex = 0;

	//	Create the listbox object
	this->entityPanel_EntityList = (gcnew System::Windows::Forms::ListBox());
	this->entityPanel_EntityList->FormattingEnabled = true;
	this->entityPanel_EntityList->Location = System::Drawing::Point(4, 4);
	this->entityPanel_EntityList->Name = L"EntityListBox";
	this->entityPanel_EntityList->Size = System::Drawing::Size(193, 225);
	this->entityPanel_EntityList->TabIndex = 0;
	this->entityPanel_EntityList->SelectedIndexChanged += gcnew System::EventHandler(this, &MainWindow::entityPanel_EntityList_SelectedIndexChanged);


	//	Hook up
	this->entityPanel->Controls->Add(this->entityPanel_EntityList);
	this->Controls->Add(this->entityPanel);

	this->UpdateEntityPanelList();

	//	Some weird shit
	this->SuspendLayout();
}
#pragma endregion


#pragma region Update Entity Panel List
void MainWindow::UpdateEntityPanelList()
{
	this->entityPanel_EntityList->ClearSelected();
	this->entityPanel_EntityList->Items->Clear();

	unsigned int entityCount = m_world->GetEntityCount();
	for (unsigned int n = 0; n < entityCount; ++n)
	{
		if (m_world->IsEntityAlive(n))
		{
			std::string entryLabel = std::to_string(n);

			if (m_world->HasComponent(n, "EntityTag"))
			{
				char*	entityTag = (char*)m_world->GetComponent(n, "EntityTag", "Tag");

				entryLabel += " | ";
				entryLabel += std::string(entityTag);
			}

			this->entityPanel_EntityList->Items->Add(gcnew System::String(entryLabel.c_str()));
		}
	}
}
#pragma endregion

#pragma region Entity List events
System::Void MainWindow::entityPanel_EntityList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
{
	unsigned int entityIndex = std::atoi(toString(entityPanel_EntityList->SelectedItem).c_str());
	unsigned int listIndex = std::atoi(toString(entityPanel_EntityList->SelectedIndex).c_str());

	UpdateComponentPanelList(entityIndex);
}
#pragma endregion

#pragma endregion

#pragma region Component Panel section

#pragma region Create Component Panel
void MainWindow::CreateComponentPanel()
{
	//	Create the panel object
	this->componentPanel = (gcnew System::Windows::Forms::Panel());
	this->componentPanel->SuspendLayout();

	this->componentPanel->Location = System::Drawing::Point(223, 13);
	this->componentPanel->Name = L"ComponentPanel";
	this->componentPanel->Size = System::Drawing::Size(200, 335);
	this->componentPanel->TabIndex = 0;

	//	Create the listbox object
	this->componentPanel_ComponentList = (gcnew System::Windows::Forms::ListBox());
	this->componentPanel_ComponentList->FormattingEnabled = true;
	this->componentPanel_ComponentList->Location = System::Drawing::Point(4, 4);
	this->componentPanel_ComponentList->Name = L"ComponentListBox";
	this->componentPanel_ComponentList->Size = System::Drawing::Size(193, 225);
	this->componentPanel_ComponentList->TabIndex = 0;
	//this->componentPanel_ComponentList->SelectedIndexChanged += gcnew System::EventHandler(this, &MainWindow::component);

	//	Create the remove button
	this->componentPanel_RemoveComponentButton = (gcnew System::Windows::Forms::Button());
	this->componentPanel_RemoveComponentButton->Name = L"RemoveComponentButton";
	this->componentPanel_RemoveComponentButton->Text = L"Remove";
	this->componentPanel_RemoveComponentButton->Size = System::Drawing::Size(50, 20);
	this->componentPanel_RemoveComponentButton->Location = System::Drawing::Point(4, 230);

	this->componentPanel_RemoveComponentButton->Click += gcnew System::EventHandler(this, &MainWindow::RemoveComponent);

	//	Hook up
	this->componentPanel->Controls->Add(this->componentPanel_ComponentList);
	this->componentPanel->Controls->Add(this->componentPanel_RemoveComponentButton);
	this->Controls->Add(this->componentPanel);


	//	Some weird shit
	this->SuspendLayout();
}
#pragma endregion

void MainWindow::UpdateComponentPanelList(unsigned int _entityId)
{
	this->componentPanel_ComponentList->ClearSelected();
	this->componentPanel_ComponentList->Items->Clear();

	std::vector<unsigned int> components;
	m_world->GetEntityComponents(components, _entityId);
	for (int n = 0; n < components.size(); ++n)
	{
		std::string sComponent = ECSL::ComponentTypeManager::GetInstance().GetComponentType(components.at(n))->GetName();

		this->componentPanel_ComponentList->Items->Add(gcnew System::String(sComponent.c_str()));
	}

	currentEntity = _entityId;
}

#pragma region Remove Component
void MainWindow::RemoveComponent(System::Object^ sender, System::EventArgs^ e)
{
	std::string component = toString(componentPanel_ComponentList->SelectedItem);
	m_world->RemoveComponentFrom(component, currentEntity);
}
#pragma endregion


#pragma endregion
