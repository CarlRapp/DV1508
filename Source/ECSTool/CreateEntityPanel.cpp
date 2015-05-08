#include "MainWindow.h"
#include "ECSL/Managers/ComponentTypeManager.h"
#include <SDL/SDL.h>
#include <msclr\marshal_cppstd.h>

using namespace ECSTool;

void MainWindow::CreateCreateEntityPanel()
{
	this->SuspendLayout();

	//	Create the panel object
	this->createEntityPanel = (gcnew System::Windows::Forms::Panel());
	this->createEntityPanel->SuspendLayout();

	this->createEntityPanel->Location = System::Drawing::Point(13, 13);
	this->createEntityPanel->Name = L"CreateEntityPanel";
	this->createEntityPanel->AutoSize = true;
	this->createEntityPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	this->createEntityPanel->BackColor = System::Drawing::Color::Brown;
	this->createEntityPanel->TabIndex = 0;

	this->createEntityComponents_List = (gcnew System::Windows::Forms::ListView());
	this->createEntityComponents_List->Name = L"CreateEntityComponentsList";
	this->createEntityComponents_List->Size = System::Drawing::Size(200, 200);
	this->createEntityComponents_List->Location = System::Drawing::Point(304, 4);
	//this->createEntityComponents_List->SmallImageList = this->Tool_Icons;
	this->createEntityComponents_List->TabIndex = 0;
	this->createEntityComponents_List->UseCompatibleStateImageBehavior = false;
	this->createEntityComponents_List->View = System::Windows::Forms::View::Details;
	this->createEntityComponents_List->Columns->Clear();
	this->createEntityComponents_List->Columns->Add(L"Components");
	this->createEntityComponents_List->AutoResizeColumns(ColumnHeaderAutoResizeStyle::HeaderSize);
	this->createEntityComponents_List->HeaderStyle = Windows::Forms::ColumnHeaderStyle::None;
	this->createEntityComponents_List->MultiSelect = false;
	this->createEntityComponents_List->ItemSelectionChanged += gcnew ListViewItemSelectionChangedEventHandler(this, &MainWindow::CreateEntityComponents_List_ItemSelectionChanged);

	this->createEntityAddedComponents_List = (gcnew System::Windows::Forms::ListView());
	this->createEntityAddedComponents_List->Name = L"CreateEntityComponentsList";
	this->createEntityAddedComponents_List->Size = System::Drawing::Size(200, 220);
	this->createEntityAddedComponents_List->Location = System::Drawing::Point(createEntityComponents_List->Location.X + this->createEntityComponents_List->Size.Width + 30, createEntityComponents_List->Location.Y);
	//this->createEntityAddedComponents_List->SmallImageList = this->Tool_Icons;
	this->createEntityAddedComponents_List->TabIndex = 0;
	this->createEntityAddedComponents_List->UseCompatibleStateImageBehavior = false;
	this->createEntityAddedComponents_List->View = System::Windows::Forms::View::Details;
	this->createEntityAddedComponents_List->Columns->Clear();
	this->createEntityAddedComponents_List->Columns->Add(L"Components");
	this->createEntityAddedComponents_List->AutoResizeColumns(ColumnHeaderAutoResizeStyle::HeaderSize);
	this->createEntityAddedComponents_List->HeaderStyle = Windows::Forms::ColumnHeaderStyle::None;
	this->createEntityAddedComponents_List->MultiSelect = false;
	this->createEntityAddedComponents_List->ItemSelectionChanged += gcnew ListViewItemSelectionChangedEventHandler(this, &MainWindow::CreateEntityAddedComponents_List_ItemSelectionChanged);

	this->createEntitySearch_TextBox = (gcnew System::Windows::Forms::TextBox());
	this->createEntitySearch_TextBox->Name = L"SearchComponentTextBox";
	this->createEntitySearch_TextBox->Size = System::Drawing::Size(this->createEntityComponents_List->Size.Width, 10);
	this->createEntitySearch_TextBox->Location = System::Drawing::Point(createEntityComponents_List->Location.X, createEntityComponents_List->Location.Y + this->createEntityComponents_List->Size.Height);
	this->createEntitySearch_TextBox->TabIndex = 0;

	this->createEntityPanel_AddComponentButton = (gcnew System::Windows::Forms::Button());
	this->createEntityPanel_AddComponentButton->Name = L"AddComponentButton";
	this->createEntityPanel_AddComponentButton->Size = System::Drawing::Size(this->createEntityComponents_List->Size.Width, this->createEntityPanel_AddComponentButton->Size.Height);
	this->createEntityPanel_AddComponentButton->Location = System::Drawing::Point(createEntityComponents_List->Location.X, createEntitySearch_TextBox->Location.Y + this->createEntitySearch_TextBox->Size.Height);
	this->createEntityPanel_AddComponentButton->TabIndex = 0;
	this->createEntityPanel_AddComponentButton->Text = "Add";
	this->createEntityPanel_AddComponentButton->Enabled = false;
	this->createEntityPanel_AddComponentButton->Click += gcnew System::EventHandler(this, &MainWindow::CreateEntityPanel_AddComponentButton_Clicked);

	this->createEntityPanel_RemoveComponentButton = (gcnew System::Windows::Forms::Button());
	this->createEntityPanel_RemoveComponentButton->Name = L"RemoveComponentButton";
	this->createEntityPanel_RemoveComponentButton->Size = System::Drawing::Size(this->createEntityAddedComponents_List->Size.Width, this->createEntityPanel_RemoveComponentButton->Size.Height);
	this->createEntityPanel_RemoveComponentButton->Location = System::Drawing::Point(createEntityAddedComponents_List->Location.X, createEntityAddedComponents_List->Location.Y + createEntityAddedComponents_List->Size.Height);
	this->createEntityPanel_RemoveComponentButton->TabIndex = 0;
	this->createEntityPanel_RemoveComponentButton->Text = "Remove";
	this->createEntityPanel_RemoveComponentButton->Enabled = false;
	this->createEntityPanel_RemoveComponentButton->Click += gcnew System::EventHandler(this, &MainWindow::CreateEntityPanel_RemoveComponentButton_Clicked);

	this->createEntityPanel_CreateEntityButton = (gcnew System::Windows::Forms::Button());
	this->createEntityPanel_CreateEntityButton->Name = L"CreateEntityButton";
	this->createEntityPanel_CreateEntityButton->Size = System::Drawing::Size(100, this->createEntityPanel_CreateEntityButton->Size.Height);
	this->createEntityPanel_CreateEntityButton->Location = System::Drawing::Point
		(this->createEntityAddedComponents_List->Location.X + this->createEntityAddedComponents_List->Size.Width - this->createEntityPanel_CreateEntityButton->Size.Width, 
		this->createEntityAddedComponents_List->Location.Y + this->createEntityAddedComponents_List->Size.Height + 50);
	this->createEntityPanel_CreateEntityButton->TabIndex = 0;
	this->createEntityPanel_CreateEntityButton->Text = "Create Entity";
	this->createEntityPanel_CreateEntityButton->Enabled = false;
	this->createEntityPanel_CreateEntityButton->Click += gcnew System::EventHandler(this, &MainWindow::CreateEntityPanel_CreateEntityButton_Clicked);

	this->createEntityPanel_BackButton = (gcnew System::Windows::Forms::Button());
	this->createEntityPanel_BackButton->Name = L"BackButton";
	this->createEntityPanel_BackButton->Size = System::Drawing::Size(100, this->createEntityPanel_BackButton->Size.Height);
	this->createEntityPanel_BackButton->Location = System::Drawing::Point
		(4,
		this->createEntityAddedComponents_List->Location.Y + this->createEntityAddedComponents_List->Size.Height + 50);
	this->createEntityPanel_BackButton->TabIndex = 0;
	this->createEntityPanel_BackButton->Text = "Back";

	this->createEntityPanel->Controls->Add(this->createEntityComponents_List);
	this->createEntityPanel->Controls->Add(this->createEntityAddedComponents_List);
	this->createEntityPanel->Controls->Add(this->createEntitySearch_TextBox);
	this->createEntityPanel->Controls->Add(this->createEntityPanel_AddComponentButton);
	this->createEntityPanel->Controls->Add(this->createEntityPanel_RemoveComponentButton);
	this->createEntityPanel->Controls->Add(this->createEntityPanel_CreateEntityButton);
	this->createEntityPanel->Controls->Add(this->createEntityPanel_BackButton);
	this->Controls->Add(this->createEntityPanel);

	ShowPopularComponents();
	PopulateCreateEntityLists();

	this->createEntityPanel->ResumeLayout(true);
	this->ResumeLayout(true);
}

void MainWindow::PopulateCreateEntityLists()
{
	createEntityComponents_List->BeginUpdate();

	unsigned int nComponents = ECSL::ComponentTypeManager::GetInstance().GetComponentTypeCount();

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
		componentItem->Name = gcnew System::String(componentName.c_str());

		this->createEntityComponents_List->Items->Add(componentItem);
	}

	createEntityComponents_List->Update();
	createEntityComponents_List->EndUpdate();

	m_forceUpdate = true;
}

void MainWindow::ShowPopularComponents()
{
	for (int i = 0; i < 1; ++i)
	{
		std::string componentName = ECSL::ComponentTypeManager::GetInstance().GetComponentType(i)->GetName();
		Label^ label = (gcnew System::Windows::Forms::Label());
		label->Name = gcnew System::String(componentName.c_str());
		label->Size = System::Drawing::Size(this->createEntityAddedComponents_List->Size.Width, this->createEntityPanel_RemoveComponentButton->Size.Height);
		label->Location = System::Drawing::Point(30, 30);

		this->createEntityPanel->Controls->Add(label);
	}
}

void MainWindow::CreateEntityComponents_List_ItemSelectionChanged(System::Object^ sender, ListViewItemSelectionChangedEventArgs^ e)
{
	if (e->IsSelected)
		this->createEntityPanel_AddComponentButton->Enabled = true;
	else
		this->createEntityPanel_AddComponentButton->Enabled = false;
}

void MainWindow::CreateEntityAddedComponents_List_ItemSelectionChanged(System::Object^ sender, ListViewItemSelectionChangedEventArgs^ e)
{
	if (e->IsSelected)
		this->createEntityPanel_RemoveComponentButton->Enabled = true;
	else
		this->createEntityPanel_RemoveComponentButton->Enabled = false;
}

void MainWindow::CreateEntityPanel_AddComponentButton_Clicked(System::Object^ sender, EventArgs^ e)
{
	for (int i = 0; i < this->createEntityComponents_List->SelectedItems->Count; ++i)
	{
		ListViewItem^ item = this->createEntityComponents_List->SelectedItems[i];
		this->createEntityComponents_List->Items->Remove(item);
		System::Windows::Forms::ListViewItem^ copy = (
			gcnew System::Windows::Forms::ListViewItem
			(
			gcnew cli::array< System::String^  >(1){ gcnew System::String(item->Name) }, -1
			)
			);
		copy->Name = gcnew System::String(item->Name);
		this->createEntityAddedComponents_List->Items->Add(copy);
	}

	if (this->createEntityAddedComponents_List->Items->Count > 0)
		this->createEntityPanel_CreateEntityButton->Enabled = true;
}

void MainWindow::CreateEntityPanel_RemoveComponentButton_Clicked(System::Object^ sender, EventArgs^ e)
{
	for (int i = 0; i < this->createEntityAddedComponents_List->SelectedItems->Count; ++i)
	{
		ListViewItem^ item = this->createEntityAddedComponents_List->SelectedItems[i];
		this->createEntityAddedComponents_List->Items->Remove(item);
		System::Windows::Forms::ListViewItem^ copy = (
			gcnew System::Windows::Forms::ListViewItem
			(
			gcnew cli::array< System::String^  >(1){ gcnew System::String(item->Name) }, -1
			)
			);
		copy->Name = gcnew System::String(item->Name);
		this->createEntityComponents_List->Items->Add(copy);
	}

	if (this->createEntityAddedComponents_List->Items->Count == 0)
		this->createEntityPanel_CreateEntityButton->Enabled = false;
}

void MainWindow::CreateEntityPanel_CreateEntityButton_Clicked(System::Object^ sender, EventArgs^ e)
{
	unsigned int id = m_world->CreateNewEntity();
	for (int i = 0; i < this->createEntityAddedComponents_List->Items->Count; ++i)
	{
		m_world->CreateComponentAndAddTo(toString(this->createEntityAddedComponents_List->Items[i]->Name), id);
	}
}