#ifndef __GROUP_HEADER__
#define __GROUP_HEADER__

#include "Cheets/Core/Core.h"
#include "Cheets/Core/Logger.h"
#include "Cheets/Core/Profiling.h"

#include "Cheets/Utils/UUID.hpp"

#include "Components.h"
#include "Handle.h"

#include <string>
#include <tuple>
#include <typeinfo>
#include <typeindex>
#include <type_traits>

namespace Cheets
{
	template<class ...T>
	class Group
	{
		public:
			using Tuple = std::tuple<T...>;

			Group();
			~Group();

			void Insert(T... args);

			void Replace();

		private:
			UUID m_Id;
			Tuple* m_Storage;

			// In number of tuples
			uint32_t m_StorageSize;
			uint32_t m_StorageUsed;

			std::tuple<std::vector<ComponentHandle<T>>...> m_Handles;
	};
}

#endif
