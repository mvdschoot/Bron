#include "Registry.h"

namespace Cheets
{

	Registry::Registry()
	{
		
	}

	Registry::~Registry()
	{

	}
	
	UUID& Registry::CreateEntity()
	{
		Entity ent();
		m_Entities.emplace(ent);
		return ent.p_Id;
	}
	
	template<class C>
	void Registry::AddComponent(UUID& entity, C& component)
	{
		// To do groups!

		std::set<Entity>::iterator it = m_Entities.begin();
		for (; it != m_Entities.end(); ++it)
		{
			if (entity == it->p_Id)
				break;
		}

		ComponentHandle<C> handle();
		it->AddedComponent(&handle);
	}
	
	
	template<class ...Ts>
	std::tuple<std::vector<Ts>...> Registry::Get()
	{
		
	}
	
	template<class ...Ts>
	void Registry::GroupComponents()
	{
		std::vector<decltype(m_GroupTypes)::iterator> res;
		
		for(int x = 0; x < sizeof...(Ts); x++)
		{
			auto res = IsInGroup<x, std::tuple<Ts...>>();
			if (res != m_GroupTypes.end())
			{
				m_Groups.clear(std::get<1>(*res));
				m_GroupTypes.erase(res);
			}
		}

		Group<Ts...> group;
		for(Entity& ent : m_Entities)
		{
			ent.GetComponents<Ts...>();
		}

		m_Groups.insert(group);
	}
	
	template<std::size_t I, class T>
	std::vector<std::pair<std::vector<std::type_index>, std::type_index>>::iterator Registry::IsInGroup()
	{
		std::type_index tuple_type = std::type_index(typeid(std::tuple_element_t<I, T>));
		std::vector<std::pair<std::vector<std::type_index>, std::type_index>>::iterator groupIt = m_GroupTypes.begin();
		for(; groupIt != m_GroupTypes.end(); ++groupIt)
		{
			std::vector<std::type_index>::iterator internalIt = std::get<0>(*groupIt).begin();
			for(; internalIt != std::get<0>(*groupIt).end(); ++internalIt)
			{
				if (tuple_type == *internalIt){
					return groupIt;
				}
			}
		}
		return groupIt;
	}
}
