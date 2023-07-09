#ifndef __UUID_HEADER__
#define __UUID_HEADER__

#include "Steve/Core/Logger.h"

#include <cstring>
#include <cstdlib>
#include <ostream>

namespace Steve
{
	class UUID
	{
		friend std::hash<UUID>;
		public:
			UUID() 
			{
				for(int x = 0; x < 28; x++) 
				{
					int num = (float)rand() / (float)RAND_MAX * 35.0;
					if (num <= 9)
						p_UUID[x] = '0' + num;
					else
						p_UUID[x] = 'a' + num - 10;
				}
				p_UUID[28] = 0;
			} 

			UUID(const UUID* other)
			{
				strcpy_s(p_UUID, other->p_UUID);
			}

			UUID(const UUID& other)
			{
				strcpy_s(p_UUID, other.p_UUID);
			}

			~UUID() = default;

			bool operator==(UUID& o) const
			{
				return strcmp(p_UUID, o.p_UUID) == 0;
			}

			bool operator==(const UUID& o) const
			{
				return strcmp(p_UUID, o.p_UUID) == 0;
			}

			bool operator<(const UUID& o) const
			{
				return strcmp(p_UUID, o.p_UUID) < 0;
			}

			bool operator<(UUID& o) const
			{
				return strcmp(p_UUID, o.p_UUID) < 0;
			}

			operator const std::string() const { return std::string(p_UUID); }
			operator const char*() const { return p_UUID; }

			friend std::ostream& operator<<(std::ostream & output, const UUID& D) {
				output << D.p_UUID;
				return output;
			}
			
			char p_UUID[29];
	};
}

template<>
struct std::hash<Steve::UUID>
{
	std::size_t operator()(const Steve::UUID& k) const noexcept
	{
		return std::hash<std::string>()(std::string(k.p_UUID));
	}
};

template<>
struct std::hash<const Steve::UUID>
{
	std::size_t operator()(const Steve::UUID& k) const noexcept
	{
		return std::hash<std::string>()(std::string(k.p_UUID));
	}
};


#endif
