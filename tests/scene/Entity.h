#ifndef __ENTITY_HEADER__
#define __ENTITY_HEADER__

#include "UUID.h"
#include "Component.h"
#include "ComponentHandle.h"

#include <set>
#include <map>
#include <tuple>
#include <cstring>
#include <typeindex>
#include <typeinfo>
#include <utility>

namespace Cheets
{

	class Entity
	{
		friend class Registry;

		public:
			Entity();
			~Entity();

			bool operator==(const Entity& other){return p_Id == other.p_Id;}

			template<class ...Args>
			std::tuple<Args...> GetComponents();

		private:
			void AddedComponent(IComponentHandle* handle);

		public:
			UUID p_Id;
		private:
			std::set<IComponentHandle*> m_Components;


			template<unsigned int I, class Tuple, class Components>
			struct HelperStruct {
				static void GetCompSpecialization(Tuple& tup, Components& components);
			};
			template<class Tuple, class Components>
			struct HelperStruct<0, Tuple, Components> {
				public:
					static void GetCompSpecialization(Tuple& tup, Components& components);
			};
	};
}


#endif

