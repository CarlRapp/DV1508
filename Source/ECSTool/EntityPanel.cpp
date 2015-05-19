#include "MainWindow.h"
#include "ECSL/Managers/ComponentTypeManager.h"
#include <SDL/SDL.h>
#include <msclr\marshal_cppstd.h>
using namespace ECSTool;

#pragma region Entity Panel section


#pragma region Create Entity Panel
void MainWindow::CreateEntityPanel()
{
	this->SuspendLayout();

	//	Create the panel object
	this->entityPanel = (gcnew System::Windows::Forms::Panel());
	this->entityPanel->SuspendLayout();

	this->entityPanel->Location = System::Drawing::Point(13, 13);
	this->entityPanel->Name = L"EntityPanel";
	this->entityPanel->AutoSize = true;
	this->entityPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	this->entityPanel->BackColor = System::Drawing::Color::Aqua;
	this->entityPanel->TabIndex = 0;

	//	Create the listbox object
	this->entityPanel_EntityList = (gcnew System::Windows::Forms::ListBox());
	this->entityPanel_EntityList->FormattingEnabled = true;
	this->entityPanel_EntityList->Name = L"EntityListBox";
	this->entityPanel_EntityList->Size = System::Drawing::Size(200, 200);
	this->entityPanel_EntityList->Location = System::Drawing::Point(4, 4);
	this->entityPanel_EntityList->TabIndex = 0;
	this->entityPanel_EntityList->SelectedIndexChanged += gcnew System::EventHandler(this, &MainWindow::entityPanel_EntityList_SelectedIndexChanged);

	//	Create the add component button
	this->entityPanel_FilterButton = (gcnew System::Windows::Forms::Button());
	this->entityPanel_FilterButton->Name = L"FilterButton";
	this->entityPanel_FilterButton->Text = L"Filter";
	this->entityPanel_FilterButton->Size = System::Drawing::Size(55, 20);
	this->entityPanel_FilterButton->Location = System::Drawing::Point(entityPanel_EntityList->Location.X, entityPanel_EntityList->Size.Height + 8);
	this->entityPanel_FilterButton->Click += gcnew System::EventHandler(this, &MainWindow::entityPanel_filterButton_Clicked);


	//	Hook up
	this->entityPanel->Controls->Add(this->entityPanel_EntityList);
	this->entityPanel->Controls->Add(this->entityPanel_FilterButton);

	this->Controls->Add(this->entityPanel);

	this->UpdateEntityPanelList();

	this->entityPanel->ResumeLayout(true);
	this->ResumeLayout(true);
}
#pragma endregion

#pragma region Update Entity Panel List
void MainWindow::UpdateEntityPanelList()
{
	this->entityPanel_EntityList->Items->Clear();

	unsigned int entityCount = m_world->GetEntityCount();
	unsigned int componentCount = ECSL::BitSet::GetDataTypeCount(ECSL::ComponentTypeManager::GetInstance().GetComponentTypeCount());
	
	bool entityFound = false;
	int index = -1;
	for (unsigned int n = 0; n < entityCount; ++n)
	{
		if (m_world->IsEntityAlive(n))
		{
			bool filterCheck = ECSL::BitSet::BitSetPassFilters(componentCount, m_world->GetEntityBitset(n), m_filterMandatory, m_filterRequiresOneOf, m_filterExcluded);

			if (filterCheck)
			{
				std::string entryLabel = GetEntityName(n);
				int tempIndex = this->entityPanel_EntityList->Items->Add(gcnew System::String(entryLabel.c_str()));
				
				if (m_currentEntity == n)
				{
					entityFound = true;
					index = tempIndex;
				}
			}
		}
	}

	//	If there is a entity selected, select it afterwards (if it wasnt found, set current to -1)
	if (entityFound)
	{
		this->entityPanel_EntityList->SetSelected(index, true);
		UpdateComponentPanelList(m_currentEntity);
	}
	else
		m_currentEntity = -1;

}
#pragma endregion

void MainWindow::entityPanel_filterButton_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	if (this->entityFilterPanel->Visible)
		this->entityFilterPanel->Hide();
	else
		this->entityFilterPanel->Show();
		
}

#pragma region Get Entity Name
std::string MainWindow::GetEntityName(unsigned int _eId)
{
	std::string tName = std::to_string(_eId);

	if (m_world->HasComponent(_eId, "EntityTag"))
	{
		char*	entityTag = (char*)m_world->GetComponent(_eId, "EntityTag", "Tag");

		tName += " | ";
		tName += std::string(entityTag);
	}

	return tName;
}
#pragma endregion

#pragma region Entity List events
System::Void MainWindow::entityPanel_EntityList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
{
	unsigned int entityIndex = std::atoi(toString(entityPanel_EntityList->SelectedItem).c_str());
	unsigned int listIndex = std::atoi(toString(entityPanel_EntityList->SelectedIndex).c_str());

	m_currentEntity = entityIndex;
	UpdateComponentPanelList(entityIndex);
}
#pragma endregion

#pragma endregion
