#pragma once
#include <msclr\marshal_cppstd.h>

#include "ECSL/Framework/World.h"

namespace ECSTool {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MainWindow
	/// </summary>
	public ref class MainWindow : public System::Windows::Forms::Form
	{
	public:
		MainWindow(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}
		void MainWindow::InitializeTool();
		void MainWindow::SetWorld(ECSL::World* _world);
		void MainWindow::Update(float _dt);

		
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainWindow()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Panel^  panel1;
	protected:
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::ImageList^  Tool_Icons;
	private: System::Windows::Forms::Label^  label1;
	
	private: 

		/*	ENTITY PANEL START	*/
	public:
		void MainWindow::CreateEntityPanel();
		void MainWindow::UpdateEntityPanelList();

	private:
		System::Windows::Forms::Panel^		entityPanel;
		System::Windows::Forms::ListBox^	entityPanel_EntityList;
		System::Windows::Forms::Button^		entityPanel_FilterButton;

	private:
		std::string GetEntityName(unsigned int _eId);
		System::Void entityPanel_EntityList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
		/*	ENTITY PANEL END	*/

		/*	ENTITY FILTER PANEL START	*/
	public:
		void MainWindow::CreateEntityFilterPanel();
		void MainWindow::PopulateEntityFilter();
		void MainWindow::UpdateFilter(std::string _component, int _filterType);

	private:
		System::Windows::Forms::Panel^		entityFilterPanel;
		System::Windows::Forms::ListView^	entityFilterPanel_List;
		System::Windows::Forms::Button^		entityFilterPanel_Mandatory;
		System::Windows::Forms::Button^		entityFilterPanel_RequiresOneOf;
		System::Windows::Forms::Button^		entityFilterPanel_Excluded;

	private:
		System::Void entityFilterPanel_List_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void filterButton_Clicked(System::Object^ sender, System::EventArgs^ e);
		/*	ENTITY FILTER PANEL END	*/

		/*	COMPONENT PANEL START	*/
	public:
		void MainWindow::CreateComponentPanel();
		void MainWindow::UpdateComponentPanelList(int _entityId);
		void MainWindow::RemoveComponent(System::Object^ sender, System::EventArgs^ e);

	private:
		std::string GetComponentName(unsigned int _cId);
		void ClearSelectedComponent();
		
		System::Void componentPanel_ComponentList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);

	private:
		System::Windows::Forms::Panel^		componentPanel;
		System::Windows::Forms::ListBox^	componentPanel_ComponentList;
		System::Windows::Forms::Button^		componentPanel_RemoveComponentButton;
		System::Windows::Forms::Button^		componentPanel_AddComponentButton;

	private:
		int m_currentEntity;
		int m_currentComponent;
		/*	COMPONENT PANEL END	*/


		std::string toString(System::Object^ systemString)
		{
			msclr::interop::marshal_context context;
			std::string standardString = context.marshal_as<std::string>(systemString->ToString());
			return standardString;
		}
	private: System::ComponentModel::IContainer^  components;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainWindow::typeid));
			this->Tool_Icons = (gcnew System::Windows::Forms::ImageList(this->components));
			this->SuspendLayout();
			// 
			// Tool_Icons
			// 
			this->Tool_Icons->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^>(resources->GetObject(L"Tool_Icons.ImageStream")));
			this->Tool_Icons->TransparentColor = System::Drawing::Color::Transparent;
			this->Tool_Icons->Images->SetKeyName(0, L"green_check.png");
			this->Tool_Icons->Images->SetKeyName(1, L"blue_plus.png");
			this->Tool_Icons->Images->SetKeyName(2, L"red_cross.png");
			// 
			// MainWindow
			// 
			this->ClientSize = System::Drawing::Size(784, 681);
			this->Name = L"MainWindow";
			this->ResumeLayout(false);

		}
#pragma endregion

		
		private:
			ECSL::World*			m_world;
			float					m_refreshRate;
			float					m_refreshTimer;
			bool					m_forceUpdate;
			ECSL::BitSet::DataType* m_filterMandatory;
			ECSL::BitSet::DataType* m_filterRequiresOneOf;
			ECSL::BitSet::DataType* m_filterExcluded;
	};
}
