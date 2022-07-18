#ifndef __ENTITY_HEADER__
#define __ENTITY_HEADER__

#include "Cheets/Core/Core.h"
#include "Cheets/Core/Logger.h"
#include "Cheets/Core/Profiling.h"

#include "Cheets/Utils/UUID.hpp"

#include "Component.h"
#include "ComponentHandle.h"

#include <set>
#include <map>
#include <tuple>
#include <cstring>
#include <typeindex>
#include <typeinfo>

namespace Cheets
{
	class Entity
	{
		friend Registry;

		public:
			Entity(std::string name);
			~Entity();

			bool operator==(const Entity& other){return m_Id == other.GetId();}

			template<class ...Args>
			std::tuple<Args&...> GetComponents();

		private:
			void AddedComponent(IComponentHandle* handle);

			template <std::size_t I, class Tuple, class First>
			void GetCompSpecialization(Tuple& tup);

			template <std::size_t I, class Tuple, class First, class Second, class ...Rest>
			void GetCompSpecialization(Tuple& tup);

		public:
			UUID p_Id;
			std::string p_Name;
		private:
			std::set<IComponentHandle*> m_Components;

	};
}


#endif

