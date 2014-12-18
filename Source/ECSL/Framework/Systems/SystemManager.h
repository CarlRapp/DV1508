#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <SDL/SDL.h>
#include <vector>
#include "SystemIdManager.h"
#include "SystemWorkGroup.h"
#include "ECSL/Framework/Components/DataManager.h"
#include "ECSL/Framework/Multithreading/RuntimeInfo.h"

namespace ECSL
{
	class DECLSPEC SystemManager
	{
	public:
		SystemManager(DataManager* _dataManager, std::vector<SystemWorkGroup*>* _systemWorkGroups);
		~SystemManager();

		void InitializeSystems();

		void UpdateSystemEntityLists(const RuntimeInfo& _runtime,
			std::vector<std::vector<System*>*>& _entityAddedRequests,
			std::vector<std::vector<System*>*>& _entityRemovedRequests);

		const std::vector<SystemWorkGroup*>* GetSystemWorkGroups() { return m_systemWorkGroups; }

	private:
		SystemIdManager* m_systemIdManager;
		DataManager* m_dataManager;
		std::vector<System*>* m_systems;
		std::vector<SystemWorkGroup*>* m_systemWorkGroups;

		void GenerateComponentFilter(System* _system, FilterType _filterType);
	};
}

#endif