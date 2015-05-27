#include "MainWindow.h"
#include "ECSL/Managers/ComponentTypeManager.h"
#include <SDL/SDL.h>
#include <msclr\marshal_cppstd.h>

using namespace ECSTool;

void MainWindow::CreateChooseEntityTypePanel(Form^ _form)
{
	createEntityForm = _form;

	_form->SuspendLayout();
	this->chooseEntityTypePanel = (gcnew System::Windows::Forms::Panel());
	this->chooseEntityTypePanel->SuspendLayout();

	this->chooseEntityTypePanel->Location = System::Drawing::Point(13, 13);
	this->chooseEntityTypePanel->Name = L"ChooseEntityTypePanel";
	this->chooseEntityTypePanel->AutoSize = true;
	this->chooseEntityTypePanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	//this->createEntityPanel->BackColor = System::Drawing::Color::Brown;
	this->chooseEntityTypePanel->TabIndex = 0;

	Label^ label = (gcnew System::Windows::Forms::Label());
	label->Name = "DescriptionLabel";
	label->Location = System::Drawing::Point(15, 5);
	label->Text = "Create from:";
	
	this->chooseEntityTypeScratchButton = (gcnew System::Windows::Forms::RadioButton());
	this->chooseEntityTypeScratchButton->Name = L"ScratchRadioButton";
	//this->chooseEntityTypeScratchButton->Size = System::Drawing::Size(this->createEntityComponents_List->Size.Width, this->createEntityPanel_AddComponentButton->Size.Height);
	this->chooseEntityTypeScratchButton->Location = System::Drawing::Point(15, 25);
	this->chooseEntityTypeScratchButton->TabIndex = 0;
	this->chooseEntityTypeScratchButton->Text = "Scratch";
	this->chooseEntityTypeScratchButton->CheckedChanged += gcnew System::EventHandler(this, &MainWindow::ChooseEntityTypePanel_RadioButton_Clicked);

	this->chooseEntityTypeTemplateButton = (gcnew System::Windows::Forms::RadioButton());
	this->chooseEntityTypeTemplateButton->Name = L"TemplateRadioButton";
	//this->chooseEntityTypeTemplateButton->Size = System::Drawing::Size(this->createEntityComponents_List->Size.Width, this->createEntityPanel_AddComponentButton->Size.Height);
	this->chooseEntityTypeTemplateButton->Location = System::Drawing::Point(15, 45);
	this->chooseEntityTypeTemplateButton->TabIndex = 0;
	this->chooseEntityTypeTemplateButton->Text = "Template";
	this->chooseEntityTypeTemplateButton->CheckedChanged += gcnew System::EventHandler(this, &MainWindow::ChooseEntityTypePanel_RadioButton_Clicked);

	chooseEntityTypeBackButton = (gcnew System::Windows::Forms::Button());
	chooseEntityTypeBackButton->Name = "BackButton";
	chooseEntityTypeBackButton->Location = System::Drawing::Point(0, 90);
	chooseEntityTypeBackButton->Text = "Back";
	chooseEntityTypeBackButton->Click += gcnew System::EventHandler(this, &MainWindow::ChooseEntityTypePanel_BackButton_Clicked);

	chooseEntityTypeNextButton = (gcnew System::Windows::Forms::Button());
	chooseEntityTypeNextButton->Name = "NextButton";
	chooseEntityTypeNextButton->Location = System::Drawing::Point(150, 90);
	chooseEntityTypeNextButton->Text = "Next";
	chooseEntityTypeNextButton->Enabled = false;
	chooseEntityTypeNextButton->Click += gcnew System::EventHandler(this, &MainWindow::ChooseEntityTypePanel_NextButton_Clicked);

	this->chooseEntityTypePanel->Controls->Add(chooseEntityTypeScratchButton);
	this->chooseEntityTypePanel->Controls->Add(chooseEntityTypeTemplateButton);
	this->chooseEntityTypePanel->Controls->Add(label);
	this->chooseEntityTypePanel->Controls->Add(chooseEntityTypeBackButton);
	this->chooseEntityTypePanel->Controls->Add(chooseEntityTypeNextButton);
	_form->Controls->Add(this->chooseEntityTypePanel);

	this->chooseEntityTypePanel->ResumeLayout(true);
	_form->ResumeLayout(true);
}

void MainWindow::CreateCreateEntityPanel(Form^ _form)
{
	_form->SuspendLayout();

	//	Create the panel object
	this->createEntityPanel = (gcnew System::Windows::Forms::Panel());
	this->createEntityPanel->SuspendLayout();

	this->createEntityPanel->Location = System::Drawing::Point(13, 13);
	this->createEntityPanel->Name = L"CreateEntityPanel";
	this->createEntityPanel->AutoSize = true;
	this->createEntityPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	//this->createEntityPanel->BackColor = System::Drawing::Color::Brown;
	this->createEntityPanel->TabIndex = 0;

	this->createEntityComponents_List = (gcnew System::Windows::Forms::ListView());
	this->createEntityComponents_List->Name = L"CreateEntityComponentsList";
	this->createEntityComponents_List->Size = System::Drawing::Size(200, 220);
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

	this->createEntityPanel_AddComponentButton = (gcnew System::Windows::Forms::Button());
	this->createEntityPanel_AddComponentButton->Name = L"AddComponentButton";
	this->createEntityPanel_AddComponentButton->Size = System::Drawing::Size(this->createEntityComponents_List->Size.Width, this->createEntityPanel_AddComponentButton->Size.Height);
	this->createEntityPanel_AddComponentButton->Location = System::Drawing::Point(createEntityComponents_List->Location.X, createEntityComponents_List->Location.Y + this->createEntityComponents_List->Size.Height);
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
	this->createEntityPanel_BackButton->Click += gcnew System::EventHandler(this, &MainWindow::CreateEntityPanel_BackButton_Clicked);

	this->createEntityPanel->Controls->Add(this->createEntityComponents_List);
	this->createEntityPanel->Controls->Add(this->createEntityAddedComponents_List);
	this->createEntityPanel->Controls->Add(this->createEntityPanel_AddComponentButton);
	this->createEntityPanel->Controls->Add(this->createEntityPanel_RemoveComponentButton);
	this->createEntityPanel->Controls->Add(this->createEntityPanel_CreateEntityButton);
	this->createEntityPanel->Controls->Add(this->createEntityPanel_BackButton);
	_form->Controls->Add(this->createEntityPanel);

	ShowPopularComponents();
	PopulateCreateEntityLists();

	this->createEntityPanel->ResumeLayout(true);
	_form->ResumeLayout(true);
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
	createEntityPanel_Labels = gcnew ArrayList();
	for (int i = 0; i < createEntityPopularComponentCount; ++i)
	{
		std::string componentName = ECSL::ComponentTypeManager::GetInstance().GetComponentType(i)->GetName();
		Label^ label = (gcnew System::Windows::Forms::Label());
		label->Name = gcnew System::String(componentName.c_str());
		label->Text = gcnew System::String(componentName.c_str());
		label->AutoSize = true;
		label->Font = gcnew System::Drawing::Font("Arial", 15.0f - (i * 0.5f));
		label->Location = FindLocation(label);
		label->MouseEnter += gcnew System::EventHandler(this, &MainWindow::CreateEntityPanel_PopularLabel_MouseEnter);
		label->MouseLeave += gcnew System::EventHandler(this, &MainWindow::CreateEntityPanel_PopularLabel_MouseLeave);
		label->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MainWindow::CreateEntityPanel_PopularLabel_MouseClick);
		
		createEntityPanel_Labels->Add(label);

		this->createEntityPanel->Controls->Add(label);
	}
}

System::Drawing::Point MainWindow::FindLocation(Label^ label)
{
	System::Drawing::Point startPos = System::Drawing::Point(130, 130);
	System::Drawing::SizeF textSize = this->CreateGraphics()->MeasureString(label->Text, label->Font);
	float t = 0;
	float a = 0.002f;
	float b = 0.004f;
	while (true)
	{
		System::Drawing::Point point = System::Drawing::Point(startPos.X + (2 * a * Math::Exp(b * t) * Math::Cos(t)), startPos.Y + (a * Math::Exp(b * t) * Math::Sin(t)));
		
		bool collision = false;
		auto e = createEntityPanel_Labels->GetEnumerator();
		while (e->MoveNext())
		{
			if (IsColliding(point, textSize, (Label^)e->Current))
			{
				collision = true;
				break;
			}
		}

		if (!collision)
		{
			return System::Drawing::Point(point.X - textSize.Width * 0.5f, point.Y - textSize.Height * 0.5f);
		}

		++t;
	}
}

bool MainWindow::IsColliding(System::Drawing::Point point, System::Drawing::SizeF size, Label^ otherLabel)
{
	System::Drawing::SizeF textSize = this->CreateGraphics()->MeasureString(otherLabel->Text, otherLabel->Font);
	//if (RectA.X1 < RectB.X2 
	// && RectA.X2 > RectB.X1 &&
	//	RectA.Y1 < RectB.Y2 
	// && RectA.Y2 > RectB.Y1)
	return ((point.X - size.Width * 0.5f) < (otherLabel->Location.X + textSize.Width)
		&& (point.X + size.Width * 0.5f) > otherLabel->Location.X
		&& (point.Y - size.Height * 0.5f) < (otherLabel->Location.Y + textSize.Height)
		&& (point.Y + size.Height * 0.5f) > otherLabel->Location.Y);
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

		auto en = createEntityPanel_Labels->GetEnumerator();
		while (en->MoveNext())
		{
			Label^ label = (Label^)en->Current;
			if (label->Name == item->Name)
			{
				label->Enabled = false;
				label->Visible = false;
			}
		}
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

		auto en = createEntityPanel_Labels->GetEnumerator();
		while (en->MoveNext())
		{
			Label^ label = (Label^)en->Current;
			if (label->Name == item->Name)
			{
				label->Enabled = true;
				label->Visible = true;
			}
		}
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

	createEntityForm->Close();
}

void MainWindow::CreateEntityPanel_BackButton_Clicked(System::Object^ sender, EventArgs^ e)
{
	createEntityForm->Controls->Clear();
	CreateChooseEntityTypePanel(createEntityForm);
}

void MainWindow::CreateEntityPanel_PopularLabel_MouseEnter(System::Object^ sender, EventArgs^ e)
{
	((Label^)sender)->ForeColor = System::Drawing::Color::Red;
}

void MainWindow::CreateEntityPanel_PopularLabel_MouseLeave(System::Object^ sender, EventArgs^ e)
{
	((Label^)sender)->ForeColor = System::Drawing::Color::Black;
}

void MainWindow::CreateEntityPanel_PopularLabel_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
{
	Label^ label = ((Label^)sender);
	System::Windows::Forms::ListViewItem^ listItem = (
		gcnew System::Windows::Forms::ListViewItem
		(
		gcnew cli::array< System::String^  >(1){ gcnew System::String(label->Name) }, -1
		)
		);
	listItem->Name = gcnew System::String(label->Name);
	this->createEntityAddedComponents_List->Items->Add(listItem);

	for (int i = 0; i < this->createEntityComponents_List->Items->Count; ++i)
	{
		if (this->createEntityComponents_List->Items[i]->Name == listItem->Name)
		{
			this->createEntityComponents_List->Items->RemoveAt(i);
		}
	}

	label->Visible = false;
	label->Enabled = false;

	if (this->createEntityAddedComponents_List->Items->Count > 0)
		this->createEntityPanel_CreateEntityButton->Enabled = true;
}

void MainWindow::ChooseEntityTypePanel_RadioButton_Clicked(System::Object^ sender, EventArgs^ e)
{
	chooseEntityTypeNextButton->Enabled = true;
}

void MainWindow::ChooseEntityTypePanel_NextButton_Clicked(System::Object^ sender, EventArgs^ e)
{
	createEntityForm->Controls->Clear();
	CreateCreateEntityPanel(createEntityForm);
}

void MainWindow::ChooseEntityTypePanel_BackButton_Clicked(System::Object^ sender, EventArgs^ e)
{
	createEntityForm->Close();
}