#ifndef DATAMAP_H
#define DATAMAP_H

#include <SDL/SDL.h>
#include <map>
#include "DataTable.h"

namespace ECSL
{
	class DECLSPEC DataMap : public DataTable
	{
	private:
		std::map<unsigned int, char*>* m_dataMap;
		unsigned int m_bytesPerRow;

	public:
		DataMap(unsigned const int _bytesPerRow);
		~DataMap();

		void Release();
		void ClearRow(unsigned const int _row);
		void ClearTable();

		const int GetRowCount() const;

		inline DataLocation GetData(unsigned const int _id) const
		{
			return (*m_dataMap)[_id];
		}
		inline DataLocation GetData(unsigned const int _id, unsigned const int _index) const
		{
			return (*m_dataMap)[_id] + _index;
		}

		inline void SetData(unsigned const int _id, void* _data, unsigned const int _byteCount)
		{
			memcpy((*m_dataMap)[_id], _data, _byteCount);
		}
		inline void SetData(unsigned const int _id, unsigned const int _index, void* _data, unsigned const int _byteCount)
		{
			memcpy((*m_dataMap)[_id] + _index, _data, _byteCount);
		}

		/* Data usage */
		const int GetBytesPerRow() const;
		const int GetMemoryAllocated() const;
	};
}
#endif