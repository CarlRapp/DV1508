#include "MainWindow.h"
#include "ECSL/Managers/ComponentTypeManager.h"
#include <SDL/SDL.h>
#include <msclr\marshal_cppstd.h>
#include <algorithm>
using namespace ECSTool;


#pragma region Data Panel section

#pragma region Create Data Panel
void MainWindow::CreateDataPanel()
{
	this->SuspendLayout();

	//	Create the panel object
	this->dataPanel = (gcnew System::Windows::Forms::Panel());
	this->dataPanel->SuspendLayout();

	this->dataPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;

	this->dataPanel->BackColor = System::Drawing::Color::WhiteSmoke;
	this->dataPanel->Location = System::Drawing::Point(this->componentPanel->Location.X + this->componentPanel->Size.Width-1, this->entityPanel->Location.Y);
	this->dataPanel->Name = L"DataPanel";
	//this->dataPanel->AutoSize = true;
	//this->dataPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	this->dataPanel->Size = System::Drawing::Size(210, 260);
	this->dataPanel->TabIndex = 0;


	//	Hook up
	//this->dataPanel->Controls->Add(this->dataPanel_List);


	this->dataPanel_Table = (gcnew System::Windows::Forms::TableLayoutPanel());
	this->dataPanel_Table->ColumnCount = 2;
	this->dataPanel_Table->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
		40)));
	this->dataPanel_Table->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
		60)));
	this->dataPanel_Table->Location = System::Drawing::Point(4, this->entityPanel_EntityList->Location.Y);;
	this->dataPanel_Table->Name = L"DataTable";
	this->dataPanel_Table->RowCount = 0;
	//this->dataPanel_Table->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
	//this->dataPanel_Table->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
	this->dataPanel_Table->Size = this->entityPanel_EntityList->Size;
	this->dataPanel_Table->TabIndex = 0;
	//this->dataPanel_Table->VerticalScroll->Maximum = 100000;
	this->dataPanel_Table->Padding = System::Windows::Forms::Padding(0, 0, 4, 0);
	this->dataPanel_Table->AutoScroll = true; 

	//BUTTONS >>

	//	Create the pause button
// 	this->dataPanel_Pause = (gcnew System::Windows::Forms::Button());
// 	this->dataPanel_Pause->Name = L"PauseButton";
// 	this->dataPanel_Pause->Text = L"Pause";
// 	this->dataPanel_Pause->Size = System::Drawing::Size(55, 20);
// 	this->dataPanel_Pause->Location = System::Drawing::Point(this->dataPanel_Table->Location.X + 0, this->dataPanel_Table->Size.Height + 34);
// 	this->dataPanel_Pause->Click += gcnew System::EventHandler(this, &MainWindow::dataPanel_Pause_Clicked);

	//	Create the reset button
	this->dataPanel_Reset = (gcnew System::Windows::Forms::Button());
	this->dataPanel_Reset->Name = L"ResetButton";
	this->dataPanel_Reset->Text = L"Reset";
	this->dataPanel_Reset->Size = System::Drawing::Size(55, 20);
	this->dataPanel_Reset->Location = System::Drawing::Point(this->dataPanel_Table->Location.X, this->dataPanel_Table->Size.Height + 34);
	this->dataPanel_Reset->Click += gcnew System::EventHandler(this, &MainWindow::dataPanel_Reset_Clicked);

	//	Create the apply button
	this->dataPanel_Apply = (gcnew System::Windows::Forms::Button());
	this->dataPanel_Apply->Name = L"ApplyButton";
	this->dataPanel_Apply->Text = L"Apply";
	this->dataPanel_Apply->Size = System::Drawing::Size(55, 20);
	this->dataPanel_Apply->Location = System::Drawing::Point(this->dataPanel_Table->Location.X + this->dataPanel_Table->Size.Width - 55, this->dataPanel_Table->Size.Height + 34);
	this->dataPanel_Apply->Click += gcnew System::EventHandler(this, &MainWindow::dataPanel_Apply_Clicked);

	//BUTTONS <<

	//LABELS >>
	this->dataListLabel = (gcnew System::Windows::Forms::Label());
	this->dataPanel->Controls->Add(this->dataListLabel);
	this->dataListLabel->Text = L"Data";
	this->dataListLabel->Location = System::Drawing::Point(0, 2);
	this->dataListLabel->Font = (gcnew System::Drawing::Font("Microsoft Sans Serif", 12.0f, System::Drawing::FontStyle::Bold));
	//LABELS <<

	//	Hook up
	this->dataPanel->Controls->Add(this->dataPanel_Table);
	//this->dataPanel->Controls->Add(this->dataPanel_Pause);
	this->dataPanel->Controls->Add(this->dataPanel_Reset);
	this->dataPanel->Controls->Add(this->dataPanel_Apply);

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
	dataPanel_Apply->Enabled = paused;
	dataPanel_Reset->Enabled = paused;

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

		ECSL::ComponentDataType dataType = component->GetDataTypes()->at(iterator->second.GetOffset());
		std::string data;

		enum Type
		{
			None, Textbox, Checkbox, Numeric, Decimal
		};

		Type type = None;

		switch (dataType)
		{
		case ECSL::ComponentDataType::INT:
			data = std::to_string(*(int*)m_world->GetComponent(_entityId, componentName, varName));
			type = Numeric;
			break;
		case ECSL::ComponentDataType::FLOAT:
			//data = std::to_string(*(float*)m_world->GetComponent(_entityId, componentName, varName));
			//std::replace(data.begin(), data.end(), '.', ',');
			type = Decimal;
			break;
		case ECSL::ComponentDataType::TEXT:
			data = m_world->GetComponent(_entityId, componentName, varName);
			type = Textbox;
			break;
		case ECSL::ComponentDataType::BOOL:
			//data = std::to_string(*(bool*)m_world->GetComponent(_entityId, componentName, varName));
			type = Checkbox;
			break;
		case ECSL::ComponentDataType::MATRIX:
			data = "Can't view this value";
			type = None;
			break;
		case ECSL::ComponentDataType::REFERENCE:
			data = std::to_string(*(int*)m_world->GetComponent(_entityId, componentName, varName));
			type = Numeric;
			break;
		case ECSL::ComponentDataType::INT64:
			data = std::to_string(*(Uint64*)m_world->GetComponent(_entityId, componentName, varName));
			type = Numeric;
			break;
		case ECSL::ComponentDataType::STRING:
			data = m_world->GetString(_entityId, componentName, iterator->second.GetOffset());
			type = Textbox;
			break;
		default:
			data = "";
			break;
		}

		System::Windows::Forms::Control^ component;

		switch (type)
		{
		case None:
		{
			System::Windows::Forms::Label^ temp = gcnew System::Windows::Forms::Label();
			temp->Text = gcnew System::String(data.c_str());
			temp->Margin = System::Windows::Forms::Padding(0, 7, 0, 0);
			temp->AutoEllipsis = true;

			component = temp;
			break;
		}
		case Textbox:
		{
			System::Windows::Forms::TextBox^ temp = gcnew System::Windows::Forms::TextBox();
			temp->Text = gcnew System::String(data.c_str());

			component = temp;
			break;
		}
		case Checkbox:
		{
			System::Windows::Forms::CheckBox^ temp = gcnew System::Windows::Forms::CheckBox();
			temp->Checked = *(bool*)m_world->GetComponent(_entityId, componentName, varName);

			component = temp;
			break;
		}
		case Numeric:
		{
			System::Windows::Forms::NumericUpDown^ temp = gcnew System::Windows::Forms::NumericUpDown();
			temp->Minimum = System::Decimal::MinValue;
			temp->Maximum = System::Decimal::MaxValue;
			temp->Text = gcnew System::String(data.c_str());

			component = temp;
			break;
		}
		case Decimal:
		{
			System::Windows::Forms::NumericUpDown^ temp = gcnew System::Windows::Forms::NumericUpDown();
			temp->DecimalPlaces = 5;
			temp->Minimum = System::Decimal::MinValue;
			temp->Maximum = System::Decimal::MaxValue;
			temp->Value = System::Decimal(*(float*)m_world->GetComponent(_entityId, componentName, varName));

			component = temp;
			break;
		}
		default:
			data = "";
			break;
		}
		component->Click += gcnew System::EventHandler(this, &MainWindow::dataPanel_Variable_Clicked);
		//component->Enabled = paused;

		System::Windows::Forms::Label^ lable = gcnew System::Windows::Forms::Label();
		lable->Text = gcnew System::String(varName.c_str());
		lable->Margin = System::Windows::Forms::Padding(0,7,0,0);
		lable->AutoEllipsis = true;
		
		System::Windows::Forms::ToolTip^ tooltip = gcnew System::Windows::Forms::ToolTip();
		tooltip->ToolTipIcon = ToolTipIcon::None;
		tooltip->IsBalloon = false;
		tooltip->ShowAlways = true;

		tooltip->SetToolTip(lable, gcnew System::String(varName.c_str()));

		//System::Windows::Forms::RowStyle^ row = gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 25);
		this->dataPanel_Table->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 25)));
		this->dataPanel_Table->Controls->Add(lable);
		this->dataPanel_Table->Controls->Add(component);
	}

	this->dataPanel_Table->Update();

}

// System::Void MainWindow::dataPanel_Pause_Clicked(System::Object^ sender, System::EventArgs^ e)
// {
// 	toggledPause = true;
// }

System::Void MainWindow::dataPanel_Reset_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	InternalUpdate(0.0f);
}

System::Void MainWindow::dataPanel_Apply_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	if (paused && m_currentEntity != -1 && m_currentComponent != -1)
	{
		auto component = ECSL::ComponentTypeManager::GetInstance().GetComponentType(m_currentComponent);
		auto variables = component->GetVariables();


		int row = 0;
		std::string componentName = component->GetName();
		auto iterator = variables->begin();
		for (; iterator != variables->end(); ++iterator)
		{
			std::string varName = iterator->second.GetName();

			ECSL::ComponentDataType dataType = component->GetDataTypes()->at(iterator->second.GetOffset());
			std::string data;

			enum Type
			{
				None, Textbox, Checkbox, Numeric, Decimal
			};

			Type type = None;

			System::Windows::Forms::Control^ component = this->dataPanel_Table->GetControlFromPosition(1, row);
			row++;
			
			switch (dataType)
			{

				case ECSL::ComponentDataType::INT:
				case ECSL::ComponentDataType::REFERENCE:
				{
					int data = (int)((System::Windows::Forms::NumericUpDown^)component)->Value;
					m_world->SetComponent(m_currentEntity, componentName, varName, &data);
					break;
				}
				
				case ECSL::ComponentDataType::FLOAT:
				{
					float data = (float)((System::Windows::Forms::NumericUpDown^)component)->Value;
					m_world->SetComponent(m_currentEntity, componentName, varName, &data);
					break;
				}

				case ECSL::ComponentDataType::TEXT:
				{
					std::string str = toString(((System::Windows::Forms::TextBox^)component)->Text);
					m_world->SetComponent(m_currentEntity, componentName, varName, (void*)str.c_str());
					break;
				}

				case ECSL::ComponentDataType::BOOL:
				{
					bool checked = ((System::Windows::Forms::CheckBox^)component)->Checked;
					m_world->SetComponent(m_currentEntity, componentName, varName, &checked);
					break;
				}

				case ECSL::ComponentDataType::MATRIX:
				{
					//Don't update.
					break;
				}

				case ECSL::ComponentDataType::INT64:
				{
					Uint64 data = System::Decimal::ToInt64(((System::Windows::Forms::NumericUpDown^)component)->Value);
					m_world->SetComponent(m_currentEntity, componentName, varName, &data);
					break;
				}

				case ECSL::ComponentDataType::STRING:
				{
					std::string str = toString(((System::Windows::Forms::TextBox^)component)->Text);
					m_world->SetString(m_currentEntity, componentName, iterator->second.GetOffset(), str.c_str());
					break;
				}

				default:
				{
					break;
				}

			}
		}
	}
}

System::Void MainWindow::dataPanel_Variable_Clicked(System::Object^ sender, System::EventArgs^ e)
{
	if (!paused)
		toggledPause = true;
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
