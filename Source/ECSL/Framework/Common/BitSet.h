#ifndef BITSET_H
#define BITSET_H

#include <SDL/SDL.h>
#include <math.h>
#include <vector>

namespace ECSL
{
	namespace BitSet
	{
		typedef unsigned __int64 DataType;

		inline DECLSPEC unsigned int GetIntByteSize()
		{
			return sizeof(DataType);
		}
		inline DECLSPEC DataType* GenerateBitSet(unsigned int _bitCount)
		{ 
			return new DataType[(_bitCount / GetIntByteSize()) + 1];
		}
		inline DECLSPEC unsigned int GetByteCount(unsigned int _bitCount) 
		{ 
			return (unsigned int)ceilf((float)_bitCount / 8);
		}
		inline DECLSPEC unsigned int GetIntCount(unsigned int _bitCount) 
		{ 
			return (unsigned int)ceilf((float)_bitCount / (GetIntByteSize() * 8));
		}

		class DECLSPEC BitSetConverter
		{
		public:
			~BitSetConverter();
			static BitSetConverter& GetInstance();

			DataType* GenerateBitmask(std::vector<unsigned int>* _numbersToConvert, unsigned int _maxNumberOfBits);
			std::vector<unsigned int>* GenerateBoolArray(DataType* _bitmask, unsigned int _bitmaskCount);


		private:
			BitSetConverter();
			DataType m_powerOfTwo[sizeof(DataType) * 8];

		};

	}
}

#endif