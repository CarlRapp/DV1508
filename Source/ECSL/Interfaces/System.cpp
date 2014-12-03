#include "System.h"

#include "ECSL/Framework/Common/ContainerHelper.h"
#include <assert.h>
#include <map>
#include <vector>

using namespace ECSL;

System::System()
{
	
}

System::~System()
{
	if (m_entitiesBitSet)
		delete(m_entitiesBitSet);

	if (m_entities)
		delete m_entities;
}

void System::InitializeEntityList()
{
	m_entities = new std::vector<unsigned int>();
	m_entitiesBitSet = BitSet::GenerateBitSet(m_dataManager->GetEntityCount());
}

void System::AddEntity(unsigned int _entityId)
{
	unsigned int bitIndex = ECSL::BitSet::GetBitIndex(_entityId);
	unsigned int bitSetIndex = ECSL::BitSet::GetBitSetIndex(_entityId);
	m_entitiesBitSet[bitSetIndex] |= ((ECSL::BitSet::DataType)1) << bitIndex;
	m_entities->push_back(_entityId);
}

void System::RemoveEntity(unsigned int _entityId)
{
	unsigned int bitIndex = ECSL::BitSet::GetBitIndex(_entityId);
	unsigned int bitSetIndex = ECSL::BitSet::GetBitSetIndex(_entityId);
	m_entitiesBitSet[bitSetIndex] &= ~(((ECSL::BitSet::DataType)1) << bitIndex);
	ContainerHelper::RemoveUniqueElement<unsigned int>(_entityId, *m_entities);
}

bool System::HasEntity(unsigned int _entityId)
{
	unsigned int bitIndex = ECSL::BitSet::GetBitIndex(_entityId);
	unsigned int bitSetIndex = ECSL::BitSet::GetBitSetIndex(_entityId);
	if (m_entitiesBitSet[bitSetIndex] & ((ECSL::BitSet::DataType)1) << bitIndex)
		return true;
	return false;
}

DataLocation System::GetComponent(unsigned int _entityId, const std::string& _componentType, const std::string& _variableName)
{
	return m_dataManager->GetComponentTable(_componentType)->GetComponent(_entityId, _variableName);
}

DataLocation System::GetComponent(unsigned int _entityId, unsigned int _componentTypeId, unsigned int _index)
{
	return m_dataManager->GetComponentTable(_componentTypeId)->GetComponent(_entityId, _index);
}

void System::SetComponent(unsigned int _entityId, const std::string& _componentType, const std::string& _variableName, void* _data)
{
	m_dataManager->GetComponentTable(_componentType)->SetComponent(_entityId, _variableName, _data);
}

void System::SetComponent(unsigned int _entityId, unsigned int _componentTypeId, unsigned int _index, void* _data, unsigned int _byteSize)
{
	m_dataManager->GetComponentTable(_componentTypeId)->SetComponent(_entityId, _index, _data, _byteSize);
}

void System::AddComponentTypeToFilter(const std::string& _componentType, FilterType _filterType)
{
	switch (_filterType)
	{
	case FilterType::Mandatory:
		m_mandatoryComponentTypes.Add(_componentType);
		break;
	case FilterType::RequiresOneOf:
		m_requiresOneOfComponentTypes.Add(_componentType);
		break;
	case FilterType::Excluded:
		m_excludedComponentTypes.Add(_componentType);
		break;
	default:
		printf("Unknown ComponentFilter passed into AddComponentToFilter [System.cpp]\n");
		break;
	}
}