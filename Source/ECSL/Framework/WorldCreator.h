#ifndef WORLDCREATOR_H
#define WORLDCREATOR_H

#include <SDL/SDL.h>
#include <vector>

#include "World.h"
#include "Systems/SystemWorkGroup.h"
#include "Components/ComponentType.h"
#include "../Managers/ComponentTypeManager.h"

namespace ECSL
{
	// Used for easier creation of a World object
	class DECLSPEC WorldCreator
	{
	private:
		bool m_worldInitialized;
		std::vector<SystemWorkGroup*>* m_systemWorkGroups;
		std::vector<int>* m_componentTypeIds;

	public:
		WorldCreator();
		~WorldCreator();

		template<typename ComponentType>
		void AddComponentType();
		void AddComponentType(std::string _componentType);

		/// Adds a system to a new group
		template<typename SystemType>
		void AddSystemToNewGroup();
		// Adds a system to the most recently added group
		template<typename SystemType>
		void AddSystemToCurrentGroup();
		// Adds a new system group
		void AddSystemGroup();

		World* InitializeWorld(unsigned int _entityCount);

		bool IsWorldInitialized() const { return m_worldInitialized; }
	};
	
	template<typename ComponentType>
	void WorldCreator::AddComponentType()
	{
		ComponentTypeManager::GetInstance().AddComponentType<ComponentType>();
		int newId = ComponentManager::GetTableId<ComponentType>();
		//	Check if the component type is already added
		for (int i = 0; i < m_componentTypeIds->size(); ++i)
			if (newId == m_componentTypeIds->at(i))
				return;

		//	Add it 
		m_componentTypeIds->push_back(newId);
	}

	template<typename SystemType>
	void WorldCreator::AddSystemToNewGroup()
	{
		m_systemWorkGroups->push_back(new SystemGroup(new SystemType()));
	}

	template<typename SystemType>
	void WorldCreator::AddSystemToCurrentGroup()
	{
		m_systemWorkGroups->at(m_systemWorkGroups->size() - 1)->AddSystem<SystemType>();
	}
}
#endif