#ifndef __REGISTRY_HEADER__
#define __REGISTRY_HEADER__

#include "Cheets/Core/Core.h"
#include "Cheets/Core/Logger.h"
#include "Cheets/Core/Profiling.h"

#include "Component.h"
#include "Entity.h"
#include "Group.h"
#include "ComponentHandle.h"

#include "Cheets/Utils/TemplateContainer.hpp"

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

			void CreateEntity(std::string name);

			template<Entity T, class C, class ...Args>
			void CreateComponent(Args&& ...args);

			template<Entity T, class C>
			void AddComponent(C& component);

			template<class ...Ts>
			std::tuple<std::vector<Ts>...> Get();

			template<class ...Ts>
			void GroupComponents();

		private:
			template<std::size_t I, class T>
			std::vector<std::pair<std::vector<std::type_index>, std::type_index>>::iterator IsInGroup();

		private:
			// First element in pair is Component types, second is group type
			std::set<std::pair<std::vector<std::type_index>, std::type_index>> m_GroupTypes;
			TemplateContainer m_Groups;

			std::set<Entity> m_Entities;
			std::set<IComponentHandle> m_Components;
	};

}


#endif

