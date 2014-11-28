#include "EntityTable.h"

#include <assert.h>

using namespace ECSL;

EntityTable::EntityTable(unsigned int _entityCount, unsigned int _componentTypeCount) : m_entityCount(_entityCount), m_componentTypeCount(_componentTypeCount)
{
	/* Each component state in every entity equals one bit, either it has the component (1) or it doesn't (0) */
	m_componentByteCount = BitSet::GetByteCount(_componentTypeCount);
	m_componentIntCount = BitSet::GetIntCount(_componentTypeCount);
	m_dataTable = new DataArray(_entityCount, 1 + m_componentIntCount * BitSet::GetIntByteSize());

	/* All entity id slots are available from the beginning */
	m_availableEntityIds = new std::stack<unsigned int>();
	for (int i = _entityCount - 1; i >= 0; --i)
		m_availableEntityIds->push(i);
}

EntityTable::~EntityTable()
{
	if (m_dataTable)
		delete(m_dataTable);

	if (m_availableEntityIds)
		delete(m_availableEntityIds);
}

void EntityTable::AddComponentTo(unsigned int _entityId, unsigned int _componentTypeId)
{
	BitSet::DataType* componentBitSet = (BitSet::DataType*)(m_dataTable->GetData(_entityId, 1));
	unsigned int bitSetIndex = BitSet::GetBitSetIndex(_componentTypeId);
	unsigned int bitIndex = BitSet::GetBitIndex(_componentTypeId);
	componentBitSet[bitSetIndex] |= (BitSet::DataType)1 << bitIndex;
}

bool EntityTable::EntityHasComponent(unsigned int _entityId, unsigned int _componentTypeId)
{
	BitSet::DataType* componentBitSet = (BitSet::DataType*)(m_dataTable->GetData(_entityId, 1));
	unsigned int bitSetIndex = BitSet::GetBitSetIndex(_componentTypeId);
	unsigned int bitIndex = BitSet::GetBitIndex(_componentTypeId);
	return (componentBitSet[bitSetIndex]) & ((BitSet::DataType)1 << (bitIndex));
}

bool EntityTable::EntityHasComponents(unsigned int _entityId, BitSet::DataType* _mandatoryMask, BitSet::DataType* _oneOfMask, BitSet::DataType* _exclusionMask)
{
	/* Component bit set for the entity */
	BitSet::DataType* componentBitSet = (BitSet::DataType*)(m_dataTable->GetData(_entityId) + 1);

	/* Checks every component filter (breaks if fails) */
	for (unsigned int i = 0; i < m_componentIntCount; ++i)
	{
		/* Entity doesn't have atleast one of the must-have components */
		if (_mandatoryMask && !( (_mandatoryMask[i] & componentBitSet[i]) == _mandatoryMask[i]) )
			return false;
		/* Entity has none of the atleast-one-of components */
		else if (_oneOfMask && ((_oneOfMask[i] & componentBitSet[i]) == 0))
			return false;
		/* Entity has atleast one of the excluded components  */
		else if (_exclusionMask && ((_exclusionMask[i] & componentBitSet[i]) != 0))
			return false;
	}
	return true;
}

unsigned int EntityTable::GenerateNewEntityId()
{
	/* Too many entities generated in the world. Increase entity count size! */
	assert(m_availableEntityIds->size() != 0);

	unsigned int id = m_availableEntityIds->top();
	m_availableEntityIds->pop();
	return id;
}

void EntityTable::AddOldEntityId(unsigned int _entityId)
{
	assert(_entityId < m_entityCount);

	m_availableEntityIds->push(_entityId);
	m_dataTable->ClearRow(_entityId);
}

void EntityTable::GetEntityComponents(std::vector<unsigned int>& _out, unsigned int _entityId)
{
	BitSet::DataType* componentBitSet = (BitSet::DataType*)(m_dataTable->GetData(_entityId, 1));
	int bitCount = BitSet::GetIntByteSize() * 8;
	for (unsigned int bitSetIndex = 0; bitSetIndex < m_componentIntCount; ++bitSetIndex)
	{
		for (unsigned int bitIndex = 0; bitIndex < bitCount; ++bitIndex)
		{
			if (componentBitSet[bitSetIndex] & ((BitSet::DataType)1 << (bitIndex)))
				_out.push_back(bitSetIndex * BitSet::GetIntByteSize() + bitIndex);
		}
	}
}