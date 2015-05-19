#include "MainWindow.h"
#include "ECSL/Managers/ComponentTypeManager.h"
#include <SDL/SDL.h>
#include <msclr\marshal_cppstd.h>
using namespace ECSTool;

#pragma region Entity Filter Panel

#pragma region Create Entity Filter Panel
void MainWindow::CreateComponentSubPanel()
{
	this->SuspendLayout();

	//	Create the panel object
	this->addComponentPanel = (gcnew System::Windows::Forms::Panel());
	this->addComponentPanel->SuspendLayout();

	this->addComponentPanel->Location = System::Drawing::Point(this->componentPanel->Location.X, this->componentPanel->Size.Height + 13);
	//this->addComponentPanel->BackColor = System::Drawing::Color::Pink;
	this->addComponentPanel->Hide();
	this->addComponentPanel->Name = L"ComponentSubPanel";
	this->addComponentPanel->AutoSize = true;
	this->addComponentPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	this->addComponentPanel->TabIndex = 0;

	//	Create the list
	this->addComponentPanel_List = (gcnew System::Windows::Forms::ListView());
	this->addComponentPanel_List->Name = L"ComponentSubPanelList";
	this->addComponentPanel_List->Size = this->entityPanel_EntityList->Size;
	this->addComponentPanel_List->Location = System::Drawing::Point(4, 4);
	this->addComponentPanel_List->SmallImageList = this->Tool_Icons;
	this->addComponentPanel_List->TabIndex = 0;
	this->addComponentPanel_List->UseCompatibleStateImageBehavior = false;
	this->addComponentPanel_List->View = System::Windows::Forms::View::Details;
	this->addComponentPanel_List->Columns->Clear();
	this->addComponentPanel_List->Columns->Add(L"Components");
	this->addComponentPanel_List->AutoResizeColumns(ColumnHeaderAutoResizeStyle::HeaderSize);
	this->addComponentPanel_List->HeaderStyle = Windows::Forms::ColumnHeaderStyle::None;

	this->addComponentPanel_List->MultiSelect = false;
	//this->addComponentPanel_List->SelectedIndexChanged += gcnew System::EventHandler(this, &MainWindow::entityFilterPanel_List_SelectedIndexChanged);

	//addComponentPanel_Select;
	//addComponentPanel_Apply;

	//	Create the remove component button
	this->addComponentPanel_Apply = (gcnew System::Windows::Forms::Button());
	this->addComponentPanel_Apply->Name = L"ApplyAddedComponents";
	this->addComponentPanel_Apply->Text = L"Apply";
	this->addComponentPanel_Apply->Size = System::Drawing::Size(55, 20);
	this->addComponentPanel_Apply->Location = System::Drawing::Point(this->addComponentPanel_List->Location.X + this->addComponentPanel_List->Size.Width - this->componentPanel_RemoveComponentButton->Size.Width, this->componentPanel_ComponentList->Size.Height + 8);
	this->addComponentPanel_Apply->Click += gcnew System::EventHandler(this, &MainWindow::addComponentPanel_Apply_Clicked);

	//	Create the add component button
	this->addComponentPanel_Select = (gcnew System::Windows::Forms::Button());
	this->addComponentPanel_Select->Name = L"SelectComponent";
	this->addComponentPanel_Select->Text = L"Select";
	this->addComponentPanel_Select->Size = System::Drawing::Size(55, 20);
	this->addComponentPanel_Select->Location = System::Drawing::Point(this->addComponentPanel_List->Location.X, this->addComponentPanel_List->Size.Height + 8);
	this->addComponentPanel_Select->Click += gcnew System::EventHandler(this, &MainWindow::addComponentPanel_Select_Clicked);

	//	Hook up
	this->addComponentPanel->Controls->Add(this->addComponentPanel_List);
	this->addComponentPanel->Controls->Add(this->addComponentPanel_Apply);
	this->addComponentPanel->Controls->Add(this->addComponentPanel_Select);

	this->Controls->Add(this->addComponentPanel);

	this->addComponentPanel->ResumeLayout(true);
	this->ResumeLayout(true);
}
#pragma endregion

System::Void MainWindow::addComponentPanel_Select_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	System::Windows::Forms::ListView::SelectedListViewItemCollection^ tempCollection = addComponentPanel_List->SelectedItems;
	if (tempCollection->Count == 0)
		return;

	addComponentPanel_List->BeginUpdate();
	int currentState = tempCollection[0]->ImageIndex;

	tempCollection[0]->ImageIndex = currentState == -1 ? 0 : -1;
	tempCollection[0]->Selected = true;

	addComponentPanel_List->Update();
	addComponentPanel_List->EndUpdate();

	this->m_forceUpdate = true;
}

System::Void MainWindow::addComponentPanel_Apply_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	if (m_currentEntity == -1)
		return;

	addComponentPanel_List->BeginUpdate();
	System::Windows::Forms::ListView::ListViewItemCollection^ allItems = addComponentPanel_List->Items;
	for (int n = 0; n < allItems->Count; ++n)
	{
		if (allItems[n]->ImageIndex != -1)
		{
			std::string sComponent = toString(allItems[n]->Tag);
			m_world->CreateComponentAndAddTo(sComponent, m_currentEntity);

			allItems[n]->ImageIndex = -1;
			addComponentPanel_List->Items->Remove(allItems[n]);
		}
	}
	addComponentPanel_List->EndUpdate();

	addComponentPanel->Hide();

	m_forceUpdate = true;
}

void MainWindow::PopulateComponentSubPanel(unsigned int _eId)
{
	if (_eId == -1)
		return;

	unsigned int nComponents = ECSL::ComponentTypeManager::GetInstance().GetComponentTypeCount();

	addComponentPanel_List->BeginUpdate();
	for (int n = 0; n < nComponents; ++n)
	{
		if (m_world->HasComponent(_eId, n))
			continue;

		int iconStyle = -1;
		unsigned int currentBit = n;
		/*	Calculate at which index the current number should be represented	*/
		unsigned int bitSetIndex = ECSL::BitSet::GetBitSetIndex(currentBit);
		unsigned int bitIndex = ECSL::BitSet::GetBitIndex(currentBit);

		std::string componentName = ECSL::ComponentTypeManager::GetInstance().GetComponentType(n)->GetName();
		System::Windows::Forms::ListViewItem^ componentItem = (
			gcnew System::Windows::Forms::ListViewItem
			(
				gcnew cli::array< System::String^  >(1){ gcnew System::String(componentName.c_str()) }, iconStyle
			)
			);
		componentItem->Tag = gcnew System::String(componentName.c_str());
		componentItem->Name = gcnew System::String(componentName.c_str());

		this->addComponentPanel_List->Items->Add(componentItem);
	}
	addComponentPanel_List->EndUpdate();
}