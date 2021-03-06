#include "World.h"

#include "MPL/Managers/TaskManager.h"
#include "Simulation/MTSimulation.h"
#include "Simulation/SQSimulation.h"
#include "../Managers/ComponentTypeManager.h"
#include "../Managers/EntityTemplateManager.h"

using namespace ECSL;

World::World(unsigned int _entityCount, std::vector<SystemWorkGroup*>* _systemWorkGroups, std::vector<unsigned int>* _componentTypeIds)
{
	m_dataManager = new DataManager(_entityCount, _componentTypeIds);
	m_systemManager = new SystemManager(m_dataManager, _systemWorkGroups);
	m_messageManager = new MessageManager(m_systemManager);

	m_dataManager->InitializeTables();
	m_systemManager->InitializeSystems();	
	m_messageManager->Initialize();

	/* If slave count is 0 use sequential simulation, else use multi-threaded simulation */
	if (MPL::TaskManager::GetInstance().GetSlaveCount() == 0)
		m_simulation = new SQSimulation();
	else
		m_simulation = new MTSimulation();
	m_simulation->Initialize(m_dataManager, m_systemManager, m_messageManager);

	m_dataLogger = &DataLogger::GetInstance();
}

World::~World()
{
	delete(m_simulation);
	delete(m_messageManager);
	delete(m_dataManager);
	delete(m_systemManager);
}

void World::PostInitializeSystems()
{
	m_systemManager->PostInitializeSystems();
}

void World::Update(float _dt)
{
	/* Set the logger to currently log this world */
	m_dataLogger->SetCurrentWorld(m_dataManager->GetEntityCount(), m_dataManager->GetEntityTable(), m_dataManager->GetComponentTables(), m_systemManager->GetSystemWorkGroups());

	/* Update systems, etc. */
	m_simulation->Update(_dt);
}

unsigned int World::CreateNewEntity()
{
	return m_dataManager->CreateNewEntity();
}

unsigned int World::CreateNewEntity(const std::string& _templateName)
{
	unsigned int newId = CreateNewEntity();
	EntityTemplate* entityTemplate = EntityTemplateManager::GetInstance().GetTemplate(_templateName);
	std::map<std::string, std::vector<TemplateEntry*>>* _components = entityTemplate->GetComponents();

	for (auto component : *_components)
	{
		std::string componentType = component.first;
		CreateComponentAndAddTo(componentType, newId);

		if (component.second.size() != 0)
		{
			auto componentData = component.second;
			int byteOffset = 0;
			for (int n = 0; n < componentData.size(); ++n)
			{
				if (componentData[n]->GetDataType() == ComponentDataType::INT)
				{
					int* dataLoc = (int*)GetComponent(newId, componentType, byteOffset);
					dataLoc[0] = componentData[n]->GetIntData();
					byteOffset += sizeof(int);
				}
				else if (componentData[n]->GetDataType() == ComponentDataType::FLOAT)
				{
					float* dataLoc = (float*)GetComponent(newId, componentType, byteOffset);
					dataLoc[0] = componentData[n]->GetFloatData();
					byteOffset += sizeof(float);
				}
				else if (componentData[n]->GetDataType() == ComponentDataType::REFERENCE)
				{
					int* dataLoc = (int*)GetComponent(newId, componentType, byteOffset);
					dataLoc[0] = componentData[n]->GetIntData();
					byteOffset += sizeof(int);
				}
				else if (componentData[n]->GetDataType() == ComponentDataType::TEXT)
				{
					char* dataLoc = (char*)GetComponent(newId, componentType, byteOffset);
					std::string textData = componentData[n]->GetTextData();
					for (int i = 0; i < textData.size(); ++i)
						dataLoc[i] = textData[i];
					dataLoc[textData.size()] = '\0';
					byteOffset += CHARSIZE * sizeof(char);
				}
				else if (componentData[n]->GetDataType() == ComponentDataType::BOOL)
				{
					bool* dataLoc = (bool*)GetComponent(newId, componentType, byteOffset);
					dataLoc[0] = componentData[n]->GetBoolData();
					byteOffset += sizeof(bool);
				}

			}
		}
	}

	return newId;
}

void World::KillEntity(unsigned int _entityId)
{
	m_dataManager->RemoveEntity(_entityId);
}

void World::CreateComponentAndAddTo(const std::string& _componentType, unsigned int _entityId)
{
	m_dataManager->CreateComponentAndAddTo(_componentType, _entityId);
}

void World::RemoveComponentFrom(const std::string& _componentType, unsigned int _entityId)
{
	m_dataManager->RemoveComponentFrom(_componentType, _entityId);
}

DataLocation World::GetComponent(unsigned int _entityId, const std::string& _componentType, const std::string& _variableName)
{
	return m_dataManager->GetComponentTable(_componentType)->GetComponent(_entityId, _variableName);
}

DataLocation World::GetComponent(unsigned int _entityId, const std::string& _componentType, const int _index)
{
	return m_dataManager->GetComponentTable(_componentType)->GetComponent(_entityId, _index);
}

DataLocation World::GetComponent(unsigned int _entityId, const unsigned int _componentType, const int _index)
{
	return m_dataManager->GetComponentTable(_componentType)->GetComponent(_entityId, _index);
}

void World::GetComponents(std::vector<DataLocation>& _out, const std::vector<unsigned int>* _entityIds, const std::string& _componentType, const std::string& _variableName)
{
	return m_dataManager->GetComponentTable(_componentType)->GetComponents(_out, _entityIds, _variableName);
}

void World::GetComponents(std::vector<DataLocation>& _out, const std::vector<unsigned int>* _entityIds, const std::string& _componentType, unsigned int _index)
{
	return m_dataManager->GetComponentTable(_componentType)->GetComponents(_out, _entityIds, _index);
}

void World::GetComponents(std::vector<DataLocation>& _out, const std::vector<unsigned int>* _entityIds, unsigned int _componentTypeId, unsigned int _index)
{
	return m_dataManager->GetComponentTable(_componentTypeId)->GetComponents(_out, _entityIds, _index);
}

void World::GetComponents(std::vector<DataLocation>& _out, const unsigned int* _entities, unsigned int _entityCount, const std::string& _componentType, const std::string& _variableName)
{
	return m_dataManager->GetComponentTable(_componentType)->GetComponents(_out, _entities, _entityCount, _variableName);
}

void World::GetComponents(std::vector<DataLocation>& _out, const unsigned int* _entities, unsigned int _entityCount, const std::string& _componentType, unsigned int _index)
{
	return m_dataManager->GetComponentTable(_componentType)->GetComponents(_out, _entities, _entityCount, _index);
}

void World::GetComponents(std::vector<DataLocation>& _out, const unsigned int* _entities, unsigned int _entityCount, unsigned int _componentTypeId, unsigned int _index)
{
	return m_dataManager->GetComponentTable(_componentTypeId)->GetComponents(_out, _entities, _entityCount, _index);
}

void World::SetComponent(unsigned int _entityId, const std::string& _componentType, const std::string& _variableName, void* _data, bool _notifyNetwork)
{
	ComponentTable* componentTable = m_dataManager->GetComponentTable(ComponentTypeManager::GetInstance().GetTableId(_componentType));
	componentTable->SetComponent(_entityId, _variableName, _data);
	ComponentHasChanged(_entityId, _componentType, _notifyNetwork);
}

bool World::HasComponent(unsigned int _entityId, const std::string& _componentType)
{
	unsigned int componentTypeId = ECSL::ComponentTypeManager::GetInstance().GetTableId(_componentType);
	return HasComponent(_entityId, componentTypeId);
}

bool World::HasComponent(unsigned int _entityId, unsigned int _componentTypeId)
{
	return m_dataManager->HasComponent(_entityId, _componentTypeId);
}

void World::GetEntityComponents(std::vector<unsigned int>& _out, unsigned int _entityId)
{
	m_dataManager->GetEntityTable()->GetEntityComponents(_out, _entityId);
}

void World::ComponentHasChanged(unsigned int _entityId, std::string _componentType, bool _notifyNetwork)
{
	unsigned int componentTypeId = ECSL::ComponentTypeManager::GetInstance().GetTableId(_componentType);
	ComponentHasChanged(_entityId, componentTypeId, _notifyNetwork);
}

void World::ComponentHasChanged(unsigned int _entityId, unsigned int _componentTypeId, bool _notifyNetwork)
{
	int bitSetIndex = ECSL::BitSet::GetBitSetIndex(_componentTypeId);
	int bitIndex = ECSL::BitSet::GetBitIndex(_componentTypeId);
	ECSL::BitSet::DataType* changedComponents = (ECSL::BitSet::DataType*)GetComponent(_entityId, "ChangedComponents", 0);
	changedComponents[bitSetIndex] |= ((ECSL::BitSet::DataType)1) << bitIndex;

	if (_notifyNetwork)
	{
		ECSL::BitSet::DataType* changedComponentsNetwork = (ECSL::BitSet::DataType*)GetComponent(_entityId, "ChangedComponentsNetwork", 0);
		changedComponentsNetwork[bitSetIndex] |= ((ECSL::BitSet::DataType)1) << bitIndex;
	}
}

unsigned int World::GetMemoryUsage()
{
	return m_dataManager->GetMemoryAllocated();
}

void World::LogWorldData()
{
	m_dataLogger->SetCurrentWorld(m_dataManager->GetEntityCount(), m_dataManager->GetEntityTable(), m_dataManager->GetComponentTables(), m_systemManager->GetSystemWorkGroups());
	m_dataLogger->LogWorldData();
}


//String?

void World::SetString(unsigned int _eId, const std::string& _componentType, const unsigned int _index, const char* _value)
{
	unsigned int componentTypeId = ECSL::ComponentTypeManager::GetInstance().GetTableId(_componentType);
	SetString(_eId, componentTypeId, _index, _value);
}

void World::SetString(unsigned int _eId, unsigned int _componentTypeId, const unsigned int _index, const char* _value)
{
	m_dataManager->SetString(_eId, _componentTypeId, _index, _value);
}


std::string World::GetString(unsigned int _eId, const std::string& _componentType, const unsigned int _index)
{
	unsigned int componentTypeId = ECSL::ComponentTypeManager::GetInstance().GetTableId(_componentType);
	return GetString(_eId, componentTypeId, _index);
}

std::string World::GetString(unsigned int _eId, unsigned int _componentTypeId, const unsigned int _index)
{
	return m_dataManager->GetString(_eId, _componentTypeId, _index);
}