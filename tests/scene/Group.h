#ifndef __GROUP_HEADER__
#define __GROUP_HEADER__

#include "UUID.h"
#include "Component.h"
#include "ComponentHandle.h"

#include <string>
#include <tuple>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include <functional>

namespace Cheets
{
	template<class ...T>
	class Group
	{
		public:
			using Tuple = std::tuple<T...>;

			Group() {};
			~Group() {};

			void Insert(ComponentHandle<T>... args);
			void Replace();

		private:
			template<class Tup, class R, class ...Args>
			std::vector<R> Loop(std::function<R(Args...)> func, Tup input);

			UUID m_Id;
			void* m_Storage;

			// In number of tuples
			uint32_t m_StorageSize;
			uint32_t m_StorageUsed;

			std::tuple<std::vector<IComponentHandle*>> m_Handles;
	};
}

#endif
