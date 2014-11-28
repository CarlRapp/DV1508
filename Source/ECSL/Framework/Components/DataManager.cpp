#include "DataManager.h"

#include <assert.h>
#include "../../Managers/ComponentTypeManager.h"
#include "Tables/DataArray.h"

using namespace ECSL;

DataManager::DataManager(unsigned int _entityCount, std::vector<unsigned int>* _componentTypeIds) : m_entityCount(_entityCount), m_componentTypeIds(_componentTypeIds)
{
}

DataManager::~DataManager()
{
	for (int n = m_componentTables->size() - 1; n >= 0; --n)
	{
		m_componentTables->erase(n);
	}
		
	delete m_componentTables;

	delete m_entityTable;
	delete m_componentTypeIds;
}

void DataManager::InitializeTables()
{
	m_componentTables = new std::map<unsigned int, DataTable*>();
	m_entityTable = new EntityTable(m_entityCount, m_componentTypeIds->size());

	for (unsigned int n = 0; n < m_componentTypeIds->size(); ++n)
	{
		unsigned int componentTypeId = m_componentTypeIds->at(n);
		ComponentType* componentType = ComponentTypeManager::GetInstance().GetComponentType(componentTypeId);

		/* Couldn't find component type. Is the type spelled correctly? */
		assert(componentType);

		switch (componentType->GetTableType())
		{
		case TableType::Array:
			m_componentTables->insert(std::pair<unsigned int, DataTable*>(componentTypeId, new DataArray(m_entityCount, componentType->GetByteSize())));
			break;

		case TableType::Map:
			printf("WARNING: Map not implemented yet! (ID %d)\n", m_componentTypeIds->at(n));
			break;

		default:
			printf("ERROR: Invalid Table Type! (ID %d)\n", m_componentTypeIds->at(n));
			break;
		}
	}
}

int DataManager::CreateNewEntity()
{
	return m_entityTable->GenerateNewEntityId();
}

void DataManager::CreateComponentAndAddTo(const std::string& _componentType, unsigned int _id)
{
	unsigned int tableId = ComponentTypeManager::GetInstance().GetTableId(_componentType);
	m_componentTables->at(tableId);
}

void DataManager::KillEntity(unsigned int _id)
{
	m_entityTable->AddOldEntityId(_id);
}