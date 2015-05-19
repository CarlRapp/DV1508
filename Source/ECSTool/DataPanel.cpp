#include "MainWindow.h"
#include "ECSL/Managers/ComponentTypeManager.h"
#include <SDL/SDL.h>
#include <msclr\marshal_cppstd.h>
using namespace ECSTool;


#pragma region Data Panel section

#pragma region Create Data Panel
void MainWindow::CreateDataPanel()
{
	this->SuspendLayout();

	//	Create the panel object
	this->dataPanel = (gcnew System::Windows::Forms::Panel());
	this->dataPanel->SuspendLayout();

	this->dataPanel->BackColor = System::Drawing::Color::Yellow;
	this->dataPanel->Location = System::Drawing::Point(this->componentPanel->Location.X + this->componentPanel->Size.Width, 13);
	this->dataPanel->Name = L"DataPanel";
	this->dataPanel->AutoSize = true;
	this->dataPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	this->dataPanel->TabIndex = 0;

	//	Create the list
	this->dataPanel_List = (gcnew System::Windows::Forms::ListView());
	this->dataPanel_List->BackColor = System::Drawing::Color::White;
	this->dataPanel_List->Name = L"DataList";
	this->dataPanel_List->Size = this->entityPanel_EntityList->Size;
	this->dataPanel_List->Location = System::Drawing::Point(4, 4);
	//this->dataPanel_List->SmallImageList = this->Tool_Icons;
	this->dataPanel_List->TabIndex = 0;
	this->dataPanel_List->UseCompatibleStateImageBehavior = false;
	this->dataPanel_List->View = System::Windows::Forms::View::Details;
	this->dataPanel_List->Columns->Clear();
	this->dataPanel_List->Columns->Add(L"Variable");
	this->dataPanel_List->Columns->Add(L"Data");
	this->dataPanel_List->AutoResizeColumns(ColumnHeaderAutoResizeStyle::HeaderSize);
	this->dataPanel_List->HeaderStyle = Windows::Forms::ColumnHeaderStyle::None;

	this->dataPanel_List->MultiSelect = false;
	//this->dataPanel_List->SelectedIndexChanged += gcnew System::EventHandler(this, &MainWindow::entityFilterPanel_List_SelectedIndexChanged);

	//	Hook up
	//this->dataPanel->Controls->Add(this->dataPanel_List);


	this->dataPanel_Table = (gcnew System::Windows::Forms::TableLayoutPanel());
	this->dataPanel_Table->ColumnCount = 2;
	this->dataPanel_Table->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
		40)));
	this->dataPanel_Table->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
		60)));
	this->dataPanel_Table->Location = System::Drawing::Point(4, 4);;
	this->dataPanel_Table->Name = L"DataTable";
	this->dataPanel_Table->RowCount = 0;
	//this->dataPanel_Table->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
	//this->dataPanel_Table->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
	this->dataPanel_Table->Size = this->entityPanel_EntityList->Size;
	this->dataPanel_Table->TabIndex = 0;
	//this->dataPanel_Table->VerticalScroll->Maximum = 100000;
	this->dataPanel_Table->Padding = System::Windows::Forms::Padding(0, 0, 4, 0);
	this->dataPanel_Table->AutoScroll = true;


	//	Hook up
	this->dataPanel->Controls->Add(this->dataPanel_Table);


	this->Controls->Add(this->dataPanel);

	this->dataPanel->ResumeLayout(true);
	this->ResumeLayout(true);
}
#pragma endregion

//std::string MainWindow::GetComponentName(unsigned int _cId)
//{
//	std::string sComponent = ECSL::ComponentTypeManager::GetInstance().GetComponentType(_cId)->GetName();
//
//	return sComponent;
//}

void MainWindow::UpdateDataPanelList(int _entityId, int _currentComponent)
{
	this->dataPanel_Table->Controls->Clear();

	if (_entityId == -1 || _currentComponent == -1)
		return;

	auto component = ECSL::ComponentTypeManager::GetInstance().GetComponentType(_currentComponent);
	auto variables = component->GetVariables();

	std::string componentName = component->GetName();
	auto iterator = variables->begin();
	for (; iterator != variables->end(); ++iterator)
	{
		std::string varName = iterator->second.GetName();

		SDL_Log("offset: %d", iterator->second.GetOffset());

		ECSL::ComponentDataType dataType = component->GetDataTypes()->at(iterator->second.GetOffset());
		bool enabled = true;
		std::string data;
		switch (dataType)
		{
		case ECSL::ComponentDataType::INT:
			data = std::to_string(*(int*)m_world->GetComponent(_entityId, componentName, varName));
			break;
		case ECSL::ComponentDataType::FLOAT:
			data = std::to_string(*(float*)m_world->GetComponent(_entityId, componentName, varName));
			break;
		case ECSL::ComponentDataType::TEXT:
			data = m_world->GetComponent(_entityId, componentName, varName);
			break;
		case ECSL::ComponentDataType::BOOL:
			data = std::to_string(*(bool*)m_world->GetComponent(_entityId, componentName, varName));
			break;
		case ECSL::ComponentDataType::MATRIX:
			data = "Can't view this value";
			enabled = false;
			break;
		case ECSL::ComponentDataType::REFERENCE:
			data = std::to_string(*(int*)m_world->GetComponent(_entityId, componentName, varName));
			break;
		case ECSL::ComponentDataType::INT64:
			data = std::to_string(*(Uint64*)m_world->GetComponent(_entityId, componentName, varName));
			break;
		case ECSL::ComponentDataType::STRING:
			data = m_world->GetString(_entityId, componentName, iterator->second.GetOffset());
			break;
		default:
			data = "";
			break;
		}

		System::Windows::Forms::Label^ lable = gcnew System::Windows::Forms::Label();
		lable->Text = gcnew System::String(varName.c_str());
		lable->AutoEllipsis = true;
		
		System::Windows::Forms::ToolTip^ tooltip = gcnew System::Windows::Forms::ToolTip();
		tooltip->ToolTipIcon = ToolTipIcon::None;
		tooltip->IsBalloon = false;
		tooltip->ShowAlways = true;

		tooltip->SetToolTip(lable, gcnew System::String(varName.c_str()));


		System::Windows::Forms::TextBox^ textbox = gcnew System::Windows::Forms::TextBox();
		textbox->Text = gcnew System::String(data.c_str());
		textbox->Enabled = enabled;

		System::Windows::Forms::RowStyle^ row = gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 25);
		this->dataPanel_Table->Controls->Add(lable);
		this->dataPanel_Table->Controls->Add(textbox);
		//this->dataPanel_Table->RowStyles->Add(row);
	}

		//System::Windows::Forms::ListViewItem^ item = gcnew System::Windows::Forms::ListViewItem(gcnew System::String(varName.c_str()));
		//item->SubItems->Add(gcnew System::String(data.c_str()));
		//this->dataPanel_List->Items->Add(item);


	/*this->dataPanel_List->Items->Clear();

	if (_entityId == -1 || _currentComponent == -1)
		return;

	auto component = ECSL::ComponentTypeManager::GetInstance().GetComponentType(_currentComponent);
	auto variables = component->GetVariables();

	std::string componentName = component->GetName();
	auto iterator = variables->begin();
	for (; iterator != variables->end(); ++iterator)
	{
		std::string varName = iterator->second.GetName();

		SDL_Log("offset: %d", iterator->second.GetOffset());

		ECSL::ComponentDataType dataType = component->GetDataTypes()->at(iterator->second.GetOffset());

		std::string data;
		switch (dataType)
		{
		case ECSL::ComponentDataType::INT:
			data = std::to_string(*(int*)m_world->GetComponent(_entityId, componentName, varName));
			break;
		case ECSL::ComponentDataType::FLOAT:
			data = std::to_string(*(float*)m_world->GetComponent(_entityId, componentName, varName));
			break;
		case ECSL::ComponentDataType::TEXT:
			data = m_world->GetComponent(_entityId, componentName, varName);
			break;
		case ECSL::ComponentDataType::BOOL:
			data = std::to_string(*(bool*)m_world->GetComponent(_entityId, componentName, varName));
			break;
		case ECSL::ComponentDataType::MATRIX:
			data = "Can't view this value";
			break;
		case ECSL::ComponentDataType::REFERENCE:
			data = std::to_string(*(int*)m_world->GetComponent(_entityId, componentName, varName));
			break;
		case ECSL::ComponentDataType::INT64:
			data = std::to_string(*(Uint64*)m_world->GetComponent(_entityId, componentName, varName));
			break;
		case ECSL::ComponentDataType::STRING:
			data = m_world->GetString(_entityId, componentName, iterator->second.GetOffset());
			break;
		default:
			data = "";
			break;
		}

		System::Windows::Forms::ListViewItem^ item = gcnew System::Windows::Forms::ListViewItem(gcnew System::String(varName.c_str()));
		item->SubItems->Add(gcnew System::String(data.c_str()));
		this->dataPanel_List->Items->Add(item);
	}*/

	

	//std::vector<unsigned int> components;
	//m_world->GetEntityComponents(components, _entityId);
	//bool componentFound = false;
	//int index = -1;
	//for (int n = 0; n < components.size(); ++n)
	//{
	//	int tempIndex = this->componentPanel_ComponentList->Items->Add(gcnew System::String(GetComponentName(components.at(n)).c_str()));

	//	if (components.at(n) == m_currentComponent)
	//	{
	//		componentFound = true;
	//		index = tempIndex;
	//	}

	//}

	////	If there is a entity selected, select it afterwards (if it wasnt found, set current to -1)
	//if (componentFound)
	//{
	//	this->componentPanel_ComponentList->SetSelected(index, true);
	//}
	//else
	//	ClearSelectedComponent();
}

//#pragma region Selected Index
//System::Void MainWindow::componentPanel_ComponentList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
//{
//	std::string component = toString(componentPanel_ComponentList->SelectedItem);
//	m_currentComponent = ECSL::ComponentTypeManager::GetInstance().GetTableId(component);
//}
//#pragma endregion
//
//
//void MainWindow::ClearSelectedComponent()
//{
//	m_currentComponent = -1;
//}

#pragma endregion
