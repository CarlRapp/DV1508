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
	private: System::Windows::Forms::Label^  label1;
	
	private: 

		/*	ENTITY PANEL START	*/
	public:
		void MainWindow::CreateEntityPanel();
		void MainWindow::UpdateEntityPanelList();

	private:
		System::Windows::Forms::Panel^		entityPanel;
		System::Windows::Forms::ListBox^	entityPanel_EntityList;

	private:
		System::Void entityPanel_EntityList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
		/*	ENTITY PANEL END	*/

		/*	COMPONENT PANEL START	*/
	public:
		void MainWindow::CreateComponentPanel();
		void MainWindow::UpdateComponentPanelList(unsigned int _entityId);
		void MainWindow::RemoveComponent(System::Object^ sender, System::EventArgs^ e);

	private:
		System::Windows::Forms::Panel^		componentPanel;
		System::Windows::Forms::ListBox^	componentPanel_ComponentList;
		System::Windows::Forms::Button^		componentPanel_RemoveComponentButton;

	private:
		unsigned int currentEntity;
		/*	COMPONENT PANEL END	*/


		std::string toString(System::Object^ systemString)
		{
			msclr::interop::marshal_context context;
			std::string standardString = context.marshal_as<std::string>(systemString->ToString());
			return standardString;
		}


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{

			// 
			// MainWindow
			// 
			this->ClientSize = System::Drawing::Size(536, 459);
			this->Name = L"MainWindow";
			this->ResumeLayout(false);

		}
#pragma endregion

		
		private:
			ECSL::World*	m_world;
			float			m_refreshRate;
			float			m_refreshTimer;
	};
}
