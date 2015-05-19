#include "MainWindow.h"
#include "ECSL/Managers/ComponentTypeManager.h"
#include <SDL/SDL.h>
#include <msclr\marshal_cppstd.h>


using namespace ECSTool;

void MainWindow::SetWorld(ECSL::World* _world)
{
	m_world	=	_world;
	m_refreshRate = 5.2f;
	m_refreshTimer = 0;

	m_currentEntity = -1;
	m_currentComponent = -1;
	m_forceUpdate = true;

	std::vector<unsigned int> bitsetComponents;
	m_filterMandatory = ECSL::BitSet::BitSetConverter::ArrayToBitSet(bitsetComponents, ECSL::ComponentTypeManager::GetInstance().GetComponentTypeCount());
	m_filterRequiresOneOf = ECSL::BitSet::BitSetConverter::ArrayToBitSet(bitsetComponents, ECSL::ComponentTypeManager::GetInstance().GetComponentTypeCount());
	m_filterExcluded = ECSL::BitSet::BitSetConverter::ArrayToBitSet(bitsetComponents, ECSL::ComponentTypeManager::GetInstance().GetComponentTypeCount());
}

void MainWindow::Update(float _dt)
{
	m_refreshTimer += _dt;

	if (m_forceUpdate)
	{
		m_forceUpdate = false;
		InternalUpdate(_dt);
	}
	else if (m_refreshTimer >= m_refreshRate)
	{
		m_refreshTimer -= m_refreshRate;
		//	Call internal update here
		InternalUpdate(_dt);
	}

	//UpdatePicking();
}
void MainWindow::InternalUpdate(float _dt)
{
	UpdateEntityPanelList();


	//if (this->entityFilterPanel->IsAccessible)
	//	PopulateEntityFilter();

	//if (this->createEntityPanel->IsAccessible)
	//	PopulateCreateEntityLists();
}


void MainWindow::InitializeTool()
{
	this->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
	this->AutoSize = true;

	this->CreateEntityPanel();
	this->CreateEntityFilterPanel();

	this->CreateComponentPanel();
	this->CreateComponentSubPanel();

	this->CreateDataPanel();

	this->TopMost = true;
	//this->CreateCreateEntityPanel();
	m_graphics->TogglePicking();
}

void ECSTool::MainWindow::SetGraphics(Renderer::GraphicsHigh* _graphics)
{
	m_graphics = _graphics;
}

void ECSTool::MainWindow::PickingOccured()
{
	unsigned int entityCount = m_world->GetEntityCount();
	unsigned int componentCount = ECSL::BitSet::GetDataTypeCount(ECSL::ComponentTypeManager::GetInstance().GetComponentTypeCount());

	bool entityFound = false;
	int index = -1;
	for (unsigned int n = 0; n < entityCount; ++n)
	{
		if (m_world->IsEntityAlive(n))
		{
			int modelInstanceBlaha = -1;

			unsigned int modelId = ECSL::ComponentTypeManager::GetInstance().GetTableId("Render");
			if (m_world->GetEntityBitset(n)[0] & ((ECSL::BitSet::DataType)1 << modelId))
			{
				modelInstanceBlaha = *((int*)m_world->GetComponent(n, "Render", "ModelId"));

				if (modelInstanceBlaha == m_graphics->GetPickedInstanceID())
				{
					m_currentEntity = n;
					break;
				}
			}
		}
	}

	m_forceUpdate = true;
}