#pragma once

#include "Bron/Core/Logger.h"

#include <cstring>
#include <cstdlib>
#include <ostream>

namespace bron
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
						value[x] = '0' + num;
					else
						value[x] = 'a' + num - 10;
				}
				value[28] = 0;
			} 

			UUID(const UUID* other)
			{
				strcpy(value, other->value);
			}

			UUID(const UUID& other)
			{
				strcpy(value, other.value);
			}

			~UUID() = default;

			bool operator==(UUID& o) const
			{
				return strcmp(value, o.value) == 0;
			}

			bool operator==(const UUID& o) const
			{
				return strcmp(value, o.value) == 0;
			}

			bool operator<(const UUID& o) const
			{
				return strcmp(value, o.value) < 0;
			}

			bool operator<(UUID& o) const
			{
				return strcmp(value, o.value) < 0;
			}

			operator const std::string() const { return std::string(value); }
			operator const char*() const { return value; }

			friend std::ostream& operator<<(std::ostream & output, const UUID& D) {
				output << D.value;
				return output;
			}
			
			char value[29];
	};
}

template<>
struct std::hash<bron::UUID>
{
	std::size_t operator()(const bron::UUID& k) const noexcept
	{
		return std::hash<std::string>()(std::string(k.value));
	}
};

template<>
struct std::hash<const bron::UUID>
{
	std::size_t operator()(const bron::UUID& k) const noexcept
	{
		return std::hash<std::string>()(std::string(k.value));
	}
};
