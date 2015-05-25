#include "MainWindow.h"
#include "ECSL/Managers/ComponentTypeManager.h"
#include <SDL/SDL.h>
#include <msclr\marshal_cppstd.h>
using namespace ECSTool;

#pragma region Entity Filter Panel

#pragma region Create Entity Filter Panel
void MainWindow::CreateEntityFilterPanel()
{
	this->SuspendLayout();

	//	Create the panel object
	this->entityFilterPanel = (gcnew System::Windows::Forms::Panel());
	this->entityFilterPanel->SuspendLayout();

	this->entityFilterPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;

	this->entityFilterPanel->Location = System::Drawing::Point(this->entityPanel->Location.X, this->entityPanel->Location.Y + this->entityPanel->Size.Height);
	this->entityFilterPanel->BackColor = System::Drawing::Color::LightGray;
	this->entityFilterPanel->Hide();
	this->entityFilterPanel->Name = L"EntityFilterPanel";
	//this->entityFilterPanel->AutoSize = true;
	//this->entityFilterPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	this->entityFilterPanel->Size = System::Drawing::Size(210, 260);
	this->entityFilterPanel->TabIndex = 0;

	//	Create the list
	this->entityFilterPanel_List = (gcnew System::Windows::Forms::ListView());
	this->entityFilterPanel_List->Name = L"EntityFilterList";
	this->entityFilterPanel_List->Size = this->entityPanel_EntityList->Size;
	this->entityFilterPanel_List->Location = System::Drawing::Point(4, 4);
	this->entityFilterPanel_List->SmallImageList = this->Tool_Icons;
	this->entityFilterPanel_List->TabIndex = 0;
	this->entityFilterPanel_List->UseCompatibleStateImageBehavior = false;
	this->entityFilterPanel_List->View = System::Windows::Forms::View::Details;
	this->entityFilterPanel_List->Columns->Clear();
	this->entityFilterPanel_List->Columns->Add(L"Components");
	this->entityFilterPanel_List->AutoResizeColumns(ColumnHeaderAutoResizeStyle::HeaderSize);
	this->entityFilterPanel_List->HeaderStyle = Windows::Forms::ColumnHeaderStyle::None;

	//this->entityFilterPanel_List->MultiSelect = false;
	this->entityFilterPanel_List->SelectedIndexChanged += gcnew System::EventHandler(this, &MainWindow::entityFilterPanel_List_SelectedIndexChanged);

	//	Create the Mandatory button
	this->entityFilterPanel_Mandatory = (gcnew System::Windows::Forms::Button());
	this->entityFilterPanel_Mandatory->Name = L"MandatoryButton";
	this->entityFilterPanel_Mandatory->Text = L"Mandatory";
	this->entityFilterPanel_Mandatory->Size = System::Drawing::Size(66, 20);
	this->entityFilterPanel_Mandatory->Location = System::Drawing::Point(this->entityFilterPanel_List->Location.X, this->entityFilterPanel_List->Location.Y + this->entityFilterPanel_List->Size.Height + 4);
	this->entityFilterPanel_Mandatory->Click += gcnew System::EventHandler(this, &MainWindow::entityFilterPanel_filterButtons_Clicked);

	this->entityFilterPanel_RequiresOneOf = (gcnew System::Windows::Forms::Button());
	this->entityFilterPanel_RequiresOneOf->Name = L"RequiresOneOfButton";
	this->entityFilterPanel_RequiresOneOf->Text = L"One of";
	this->entityFilterPanel_RequiresOneOf->Size = this->entityFilterPanel_Mandatory->Size;
	this->entityFilterPanel_RequiresOneOf->Location = System::Drawing::Point(this->entityFilterPanel_Mandatory->Location.X + this->entityFilterPanel_Mandatory->Size.Width + 1, this->entityFilterPanel_Mandatory->Location.Y);
	this->entityFilterPanel_RequiresOneOf->Click += gcnew System::EventHandler(this, &MainWindow::entityFilterPanel_filterButtons_Clicked);

	this->entityFilterPanel_Excluded = (gcnew System::Windows::Forms::Button());
	this->entityFilterPanel_Excluded->Name = L"ExcludedButton";
	this->entityFilterPanel_Excluded->Text = L"Excluded";
	this->entityFilterPanel_Excluded->Size = this->entityFilterPanel_Mandatory->Size;
	this->entityFilterPanel_Excluded->Location = System::Drawing::Point(this->entityFilterPanel_RequiresOneOf->Location.X + this->entityFilterPanel_RequiresOneOf->Size.Width + 1, this->entityFilterPanel_RequiresOneOf->Location.Y);
	this->entityFilterPanel_Excluded->Click += gcnew System::EventHandler(this, &MainWindow::entityFilterPanel_filterButtons_Clicked);

	this->entityFilterPanel_RemoveFromFilter = (gcnew System::Windows::Forms::Button());
	this->entityFilterPanel_RemoveFromFilter->Name = L"RemoveFromFilter";
	this->entityFilterPanel_RemoveFromFilter->Text = L"Remove";
	this->entityFilterPanel_RemoveFromFilter->Size = this->entityFilterPanel_RequiresOneOf->Size;
	this->entityFilterPanel_RemoveFromFilter->Location = System::Drawing::Point(this->entityFilterPanel_Mandatory->Location.X, this->entityFilterPanel_Mandatory->Location.Y + this->entityFilterPanel_Mandatory->Size.Height + 6);
	this->entityFilterPanel_RemoveFromFilter->Click += gcnew System::EventHandler(this, &MainWindow::entityFilterPanel_filterButtons_Clicked);
	//this->entityFilterPanel_RemoveFromFilter->Location = System::Drawing::Point(this->entityFilterPanel_List->Location.X + this->entityFilterPanel_List->Size.Width - this->entityFilterPanel_RemoveFromFilter->Size.Width, this->entityFilterPanel_RequiresOneOf->Location.Y);

	//	Hook up
	this->entityFilterPanel->Controls->Add(this->entityFilterPanel_List);
	this->entityFilterPanel->Controls->Add(this->entityFilterPanel_Mandatory);
	this->entityFilterPanel->Controls->Add(this->entityFilterPanel_RequiresOneOf);
	this->entityFilterPanel->Controls->Add(this->entityFilterPanel_Excluded);
	this->entityFilterPanel->Controls->Add(this->entityFilterPanel_RemoveFromFilter);

	this->Controls->Add(this->entityFilterPanel);

	this->PopulateEntityFilter();

	this->entityFilterPanel->ResumeLayout(true);
	this->ResumeLayout(true);
}
#pragma endregion

void MainWindow::PopulateEntityFilter()
{
	//this->entityFilterPanel_List->Clear();

	unsigned int nComponents = ECSL::ComponentTypeManager::GetInstance().GetComponentTypeCount();
	
	entityFilterPanel_List->BeginUpdate();
	entityFilterPanel_List->Sorting = System::Windows::Forms::SortOrder::Ascending;
	for (int n = 0; n < nComponents; ++n)
	{
		int iconStyle = -1;
		unsigned int currentBit = n;
		/*	Calculate at which index the current number should be represented	*/
		unsigned int bitSetIndex = ECSL::BitSet::GetBitSetIndex(currentBit);
		unsigned int bitIndex = ECSL::BitSet::GetBitIndex(currentBit);
			
		if (m_filterMandatory[bitSetIndex] & ((ECSL::BitSet::DataType)1 << bitIndex))
			iconStyle = 0;
		else if (m_filterRequiresOneOf[bitSetIndex] & ((ECSL::BitSet::DataType)1 << bitIndex))
			iconStyle = 1;
		else if (m_filterExcluded[bitSetIndex] & ((ECSL::BitSet::DataType)1 << bitIndex))
			iconStyle = 2;

		std::string componentName = ECSL::ComponentTypeManager::GetInstance().GetComponentType(n)->GetName();
		System::Windows::Forms::ListViewItem^ componentItem = (
			gcnew System::Windows::Forms::ListViewItem
			(
			gcnew cli::array< System::String^  >(1){ gcnew System::String(componentName.c_str()) }, iconStyle
			)
		);
		componentItem->Tag = gcnew System::String(componentName.c_str());
		componentItem->Name = gcnew System::String(componentName.c_str());

		this->entityFilterPanel_List->Items->Add(componentItem);
	}
	entityFilterPanel_List->Sort();
	entityFilterPanel_List->EndUpdate();
}

System::Void MainWindow::entityFilterPanel_List_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
{
	System::Windows::Forms::ListView::SelectedListViewItemCollection^ tempCollection = entityFilterPanel_List->SelectedItems;
	if (tempCollection->Count == 0)
		return;
}

System::Void MainWindow::entityFilterPanel_filterButtons_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	System::Windows::Forms::ListView::SelectedListViewItemCollection^ tempCollection = entityFilterPanel_List->SelectedItems;
	if (tempCollection->Count == 0)
		return;

	entityFilterPanel_List->BeginUpdate();
	for (int n = 0; n < tempCollection->Count; ++n)
	{
		std::string currentComponent = toString(entityFilterPanel_List->SelectedItems[n]->Tag).c_str();

		System::String^ buttonName = ((System::Windows::Forms::Button^)sender)->Name;
		if (buttonName == this->entityFilterPanel_Mandatory->Name)
			UpdateFilter(currentComponent, ECSL::FilterType::Mandatory, n);
		else if (buttonName == this->entityFilterPanel_RequiresOneOf->Name)
			UpdateFilter(currentComponent, ECSL::FilterType::RequiresOneOf, n);
		else if (buttonName == this->entityFilterPanel_Excluded->Name)
			UpdateFilter(currentComponent, ECSL::FilterType::Excluded, n);
		else if (buttonName == this->entityFilterPanel_RemoveFromFilter->Name)
			UpdateFilter(currentComponent, -1, n);

		
	}
	entityFilterPanel_List->Update();
	entityFilterPanel_List->EndUpdate();
}

void MainWindow::UpdateFilter(std::string _component, int _filterType, int _listIndex)
{
	unsigned int currentBit = ECSL::ComponentTypeManager::GetInstance().GetTableId(_component);
	/*	Calculate at which index the current number should be represented	*/
	unsigned int bitSetIndex = ECSL::BitSet::GetBitSetIndex(currentBit);
	unsigned int bitIndex = ECSL::BitSet::GetBitIndex(currentBit);

	
	//	Remove component from filters
	if (_filterType == -1)
	{
		m_filterMandatory[bitSetIndex] &= ~((ECSL::BitSet::DataType)1 << bitIndex);
		m_filterRequiresOneOf[bitSetIndex] &= ~((ECSL::BitSet::DataType)1 << bitIndex);
		m_filterExcluded[bitSetIndex] &= ~((ECSL::BitSet::DataType)1 << bitIndex);
		entityFilterPanel_List->SelectedItems[_listIndex]->ImageIndex = -1;
		
	}
	else if (_filterType == ECSL::FilterType::Mandatory)
	{
		m_filterMandatory[bitSetIndex] |= ((ECSL::BitSet::DataType)1 << bitIndex);
		m_filterRequiresOneOf[bitSetIndex] &= ~((ECSL::BitSet::DataType)1 << bitIndex);
		m_filterExcluded[bitSetIndex] &= ~((ECSL::BitSet::DataType)1 << bitIndex);
		entityFilterPanel_List->SelectedItems[_listIndex]->ImageIndex = 0;
	}
	else if (_filterType == ECSL::FilterType::RequiresOneOf)
	{
		m_filterMandatory[bitSetIndex] &= ~((ECSL::BitSet::DataType)1 << bitIndex);
		m_filterRequiresOneOf[bitSetIndex] |= ((ECSL::BitSet::DataType)1 << bitIndex);
		m_filterExcluded[bitSetIndex] &= ~((ECSL::BitSet::DataType)1 << bitIndex);
		entityFilterPanel_List->SelectedItems[_listIndex]->ImageIndex = 1;
	}
	else if (_filterType == ECSL::FilterType::Excluded)
	{
		m_filterMandatory[bitSetIndex] &= ~((ECSL::BitSet::DataType)1 << bitIndex);
		m_filterRequiresOneOf[bitSetIndex] &= ~((ECSL::BitSet::DataType)1 << bitIndex);
		m_filterExcluded[bitSetIndex] |= ((ECSL::BitSet::DataType)1 << bitIndex);
		entityFilterPanel_List->SelectedItems[_listIndex]->ImageIndex = 2;
	}



	this->m_forceUpdate = true;
}




#pragma endregion
