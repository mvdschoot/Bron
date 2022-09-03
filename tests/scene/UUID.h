#ifndef __UUID_HEADER__
#define __UUID_HEADER__

#include <random>
#include <string.h>

namespace Cheets
{
	class UUID
	{
		public:
			UUID() 
			{
				for(int x = 0; x < 28; x++)
				{
					int num = m_Dist(m_Gen);
					if (num <= 9)
						m_UUID[x] = '0' + num;
					else
						m_UUID[x] = 'a' + num - 10;
				}
				m_UUID[28] = 0;
			}

			~UUID() {};

			bool operator==(UUID& o)
			{
				return strcmp(m_UUID, o.m_UUID) == 0;
			}

			bool operator==(const UUID& o)
			{
				return strcmp(m_UUID, o.m_UUID) == 0;
			}

			bool operator!=(UUID& o)
			{
				return strcmp(m_UUID, o.m_UUID) != 0;
			}

		private:
			char m_UUID[29];

			static std::random_device m_Device;
			static std::mt19937 m_Gen;
			static std::uniform_int_distribution<std::mt19937::result_type> m_Dist;
	};
}


#endif