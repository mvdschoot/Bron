#ifndef __REGISTRY_HEADER__
#define __REGISTRY_HEADER__

#include "Component.h"
#include "Entity.h"
#include "Group.h"
#include "ComponentHandle.h"

#include "TemplateContainer.hpp"

#include <string>
#include <set>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <utility>

namespace Cheets
{
	class Registry
	{
		friend Entity;

		public:
			Registry();
			~Registry();

			UUID& CreateEntity();

			template<class C>
			void AddComponent(UUID& entity, C& component);

			template<class ...Ts>
			std::tuple<std::vector<Ts>...> Get();

			template<class ...Ts>
			void GroupComponents();

		private:
			template<std::size_t I, class T>
			std::vector<std::pair<std::vector<std::type_index>, std::type_index>>::iterator IsInGroup();

		private:
			// First element in pair is Component types, second is group type
			std::vector<std::pair<std::vector<std::type_index>, std::type_index>> m_GroupTypes;
			TemplateContainer m_Groups;

			std::set<Entity> m_Entities;
			std::set<IComponentHandle> m_Components;
	};

}


#endif

