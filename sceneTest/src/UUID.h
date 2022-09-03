#ifndef __UUID_HEADER__
#define __UUID_HEADER__

#include <random>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <ostream>

namespace Cheets
{
	class UUID
	{	
		public:
			UUID() 
			{
				for(int x = 0; x < 28; x++) 
				{
					int num = (float)rand() / (float)RAND_MAX * 35.0;
					if (num <= 9)
						m_UUID[x] = '0' + num;
					else
						m_UUID[x] = 'a' + num - 10;
				}
				m_UUID[28] = 0;
			}

			UUID(const UUID* other)
			{
				strcpy(m_UUID, other->m_UUID);
			}

			~UUID() {};

			bool operator==(UUID& o) const
			{
				return strcmp(m_UUID, o.m_UUID) == 0;
			}

			bool operator==(const UUID& o) const
			{
				return strcmp(m_UUID, o.m_UUID) == 0;
			}

			bool operator<(const UUID& o) const
			{
				return strcmp(m_UUID, o.m_UUID) < 0;
			}

			bool operator<(UUID& o) const
			{
				return strcmp(m_UUID, o.m_UUID) < 0;
			}

			friend std::ostream& operator<<(std::ostream & output, const UUID& D) {
				output << D.m_UUID;
				return output;
			}

		public:
			char m_UUID[29];
	};
}

template<>
struct std::hash<Cheets::UUID>
{
	friend class UUID;
	std::size_t operator()(const Cheets::UUID& k) const
	{
		return std::hash<std::string>()(std::string(k.m_UUID));
	}
};


#endif