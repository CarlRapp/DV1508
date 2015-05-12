#include "MainWindow.h"
#include "ECSL/Managers/ComponentTypeManager.h"
#include <SDL/SDL.h>
#include <msclr\marshal_cppstd.h>


using namespace ECSTool;

void MainWindow::SetWorld(ECSL::World* _world)
{
	m_world	=	_world;
	m_refreshRate = 100.0f;
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
}
void MainWindow::InternalUpdate(float _dt)
{
	UpdateEntityPanelList();


	if (this->entityFilterPanel->IsAccessible)
		PopulateEntityFilter();

	//if (this->createEntityPanel->IsAccessible)
	//	PopulateCreateEntityLists();
}


void MainWindow::InitializeTool()
{
	this->CreateEntityPanel();
	this->CreateEntityFilterPanel();

	this->CreateComponentPanel();
	this->CreateComponentSubPanel();

	this->CreateDataPanel();

	//this->CreateCreateEntityPanel();
}

void ECSTool::MainWindow::SetGraphics(Renderer::GraphicsHigh* _graphics)
{
	m_graphics = _graphics;
}
