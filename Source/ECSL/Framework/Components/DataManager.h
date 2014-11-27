#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <SDL/SDL.h>
#include "Tables/EntityTable.h"

#include <vector>

namespace ECSL
{
	class DECLSPEC DataManager
	{
	public:
		explicit DataManager(unsigned int _entityCount, std::vector<int>* _componentTypeIds);
		~DataManager();

		void InitializeTables();

		void AddComponentType(int _componentType);
		int CreateNewEntity();
		void CreateComponentAndAddTo(std::string& _componentType, int _id);

	private:
		unsigned int m_entityCount;
		EntityTable* m_entityTable;
		std::vector<DataTable*>* m_componentTables;
		std::vector<int>* m_componentTypeIds;
	};
}

#endif