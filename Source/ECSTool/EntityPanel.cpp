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

	this->entityPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;

	this->entityPanel->Location = System::Drawing::Point(3, 28);
	this->entityPanel->Name = L"EntityPanel";
	//this->entityPanel->AutoSize = true;
	//this->entityPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	this->entityPanel->Size = System::Drawing::Size(210, 260);
	this->entityPanel->BackColor = System::Drawing::Color::WhiteSmoke;
	this->entityPanel->TabIndex = 0;

	//	Create the listbox object
	this->entityPanel_EntityList = (gcnew System::Windows::Forms::ListBox());
	this->entityPanel_EntityList->FormattingEnabled = true;
	this->entityPanel_EntityList->Name = L"EntityListBox";
	this->entityPanel_EntityList->Size = System::Drawing::Size(200, 200);
	this->entityPanel_EntityList->Location = System::Drawing::Point(4, 26);
	this->entityPanel_EntityList->TabIndex = 0;
	this->entityPanel_EntityList->SelectedIndexChanged += gcnew System::EventHandler(this, &MainWindow::entityPanel_EntityList_SelectedIndexChanged);

	//	Create the add component button
	this->entityPanel_FilterButton = (gcnew System::Windows::Forms::Button());
	this->entityPanel_FilterButton->Name = L"FilterButton";
	this->entityPanel_FilterButton->Text = L"Filter";
	this->entityPanel_FilterButton->Size = System::Drawing::Size(55, 20);
	this->entityPanel_FilterButton->Location = System::Drawing::Point(entityPanel_EntityList->Location.X, entityPanel_EntityList->Location.Y+entityPanel_EntityList->Size.Height + 8);
	this->entityPanel_FilterButton->Click += gcnew System::EventHandler(this, &MainWindow::entityPanel_filterButton_Clicked);

	//	Create new entity button
	this->entityPanel_NewEntityButton = (gcnew System::Windows::Forms::Button());
	this->entityPanel_NewEntityButton->Name = L"NewEntityButton";
	this->entityPanel_NewEntityButton->Text = L"New Entity";
	this->entityPanel_NewEntityButton->Size = System::Drawing::Size(70, 20);
	this->entityPanel_NewEntityButton->Location = System::Drawing::Point(entityPanel_FilterButton->Location.X + entityPanel_FilterButton->Size.Width + 3, entityPanel_EntityList->Location.Y + entityPanel_EntityList->Size.Height + 8);
	this->entityPanel_NewEntityButton->Click += gcnew System::EventHandler(this, &MainWindow::entityFilterPanel_newEntityButtons_Clicked);

	//	Hook up
	this->entityPanel->Controls->Add(this->entityPanel_EntityList);
	this->entityPanel->Controls->Add(this->entityPanel_FilterButton);
	this->entityPanel->Controls->Add(this->entityPanel_NewEntityButton);

	// Label
	this->entityListLabel = (gcnew System::Windows::Forms::Label());
	this->entityPanel->Controls->Add(this->entityListLabel);
	this->entityListLabel->Text = L"Entities";
	this->entityListLabel->Location = System::Drawing::Point(0, 2);
	this->entityListLabel->Font = (gcnew System::Drawing::Font("Microsoft Sans Serif", 12.0f, System::Drawing::FontStyle::Bold));
	

	this->Controls->Add(this->entityPanel);

	this->entityPanel->ResumeLayout(true);
	this->ResumeLayout(true);
}
#pragma endregion

#pragma region Update Entity Panel List
void MainWindow::UpdateEntityPanelList()
{
	this->entityPanel_EntityList->BeginUpdate();
	this->entityPanel_EntityList->Items->Clear();

	unsigned int entityCount = m_world->GetEntityCount();
	unsigned int componentCount = ECSL::BitSet::GetDataTypeCount(ECSL::ComponentTypeManager::GetInstance().GetComponentTypeCount());
	
	bool entityFound = false;
	int index = -1;
	for (unsigned int n = 0; n < entityCount; ++n)
	{
		if (m_world->IsEntityAlive(n))
		{
// 			int modelInstanceBlaha = -1;
// 
// 			unsigned int modelId = ECSL::ComponentTypeManager::GetInstance().GetTableId("Render");
// 			if (m_world->GetEntityBitset(n)[0] & ((ECSL::BitSet::DataType)1 << modelId))
// 			{
// 				modelInstanceBlaha = *((int*)m_world->GetComponent(n, "Render", "ModelId"));
// 			}

//  			if (modelInstanceBlaha == m_graphics->GetPickedInstanceID())
//  			{
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
			//}
		}
	}

	//	If there is a entity selected, select it afterwards (if it wasnt found, set current to -1)
	if (entityFound)
	{
		this->entityPanel_EntityList->SetSelected(index, true);
		UpdateComponentPanelList(m_currentEntity);
	}
	else
	{
		m_currentComponent = -1;
		m_currentEntity = -1;
		//m_graphics->SetInstanceIDToHighlight(-1);
	}

	UpdateComponentPanelList(m_currentEntity);
	this->entityPanel_EntityList->EndUpdate();
}
#pragma endregion

void MainWindow::entityPanel_filterButton_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	if (this->entityFilterPanel->Visible)
		this->entityFilterPanel->Hide();
	else
	{
		this->entityFilterPanel->Show();
		this->entityFilterPanel->Update();
	}
		
}

void MainWindow::entityFilterPanel_newEntityButtons_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	Form^ newEntityForm = gcnew Form();
	newEntityForm->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	newEntityForm->AutoSize = true;
	newEntityForm->TopMost = true;
	newEntityForm->MaximizeBox = false;
	newEntityForm->Show();
	CreateChooseEntityTypePanel(newEntityForm);
	
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
	entityPanel_EntityList->BeginUpdate();
	if (entityPanel_EntityList->SelectedItems->Count == 0)
		return;

	unsigned int entityIndex = std::atoi(toString(entityPanel_EntityList->SelectedItem).c_str());
	unsigned int listIndex = std::atoi(toString(entityPanel_EntityList->SelectedIndex).c_str());

	

	if (m_world->HasComponent(entityIndex, "Render"))
		m_graphics->SetInstanceIDToHighlight(*((int*)m_world->GetComponent(entityIndex, "Render", "ModelId")));
	else
		m_graphics->SetInstanceIDToHighlight(-1);

	UpdateComponentPanelList(entityIndex);

	m_currentEntity = entityIndex;

	entityPanel_EntityList->EndUpdate();
}
#pragma endregion

#pragma endregion
