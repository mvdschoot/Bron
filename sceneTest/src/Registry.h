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
#include <map>
#include <typeinfo>
#include <typeindex>
#include <utility>

namespace Cheets
{
	class IsInGroupException : std::exception
	{
	public:
		const char* what()
		{
			return "Type is already in a Group!";
		}
	};

	class Registry
	{
		friend class Entity;

		public:
			Registry();
			~Registry();

			UUID& CreateEntity();

			template<class ...Ts>
			void GroupComponents()
            {
                std::vector<std::type_index> types;
                ([&, this]()
                {
                    if (this->IsInGroup<Ts>() != this->m_GroupTypes.end())
                        throw IsInGroupException();
                    types.push_back(std::type_index(typeid(Ts)));
                }(), ...);

                Group<Ts...>* group = new Group<Ts...>();
                m_GroupTypes.emplace(types, std::type_index(typeid(group)));

                for (auto it = m_Entities.begin(); it != m_Entities.end(); ++it)
                {
                    group->Insert(it->second);
                }

                m_Groups.emplace(std::type_index(typeid(Group<Ts...>)), group);
            }

			template<class T>
			void AddComponent(UUID& entity, T& component)
            {
                Entity& ent = m_Entities[entity];
                ent.AddedComponent(createRef<ComponentHandle<T>>(component));
                auto groupIt = IsInGroup<T>();
                if (groupIt == m_GroupTypes.end())
                {
                    m_RestComponents.insert(component);
                    ent.AddedComponent(createRef<ComponentHandle<T>>(m_RestComponents.get<T>(ent.p_Id)));
                }
                else
                {
                    ent.AddedComponent(createRef<ComponentHandle<T>>(ent));
                }
            }

			template<class ...Ts>
			char* GetGroup()
            {
                Group<Ts...>* gr = static_cast<Group<Ts...>*>(m_Groups[std::type_index(typeid(Group<Ts...>))]);
                return gr->GetRaw();
            }

		private:
			// Checks if a type is already in a group
			template<class T>
			std::set<std::pair<std::vector<std::type_index>, std::type_index>>::iterator IsInGroup()
            {
                const auto tuple_type = std::type_index(typeid(T));
                std::set<std::pair<std::vector<std::type_index>, std::type_index>>::iterator groupIt = m_GroupTypes.begin();
                for (; groupIt != m_GroupTypes.end(); ++groupIt) {
                    std::vector<std::type_index>::const_iterator internalIt = groupIt->first.begin();
                    for (; internalIt != std::get<0>(*groupIt).end(); ++internalIt) {
                        if (tuple_type == *internalIt) {
                            return groupIt;
                        }
                    }
                }
                return groupIt;
            }

		private:
			// First element in pair is Component types, second is Group type
			std::set<std::pair<std::vector<std::type_index>, std::type_index>> m_GroupTypes;
			
			// Groups are allocated on heap
			std::unordered_map<std::type_index, IGroup*> m_Groups;

			// Entities sorted by UUID
			// Insert/Lookup O(1)
			std::unordered_map<UUID, Entity> m_Entities;

			// ungrouped components
			TemplateContainer m_RestComponents;
	};

}


#endif

