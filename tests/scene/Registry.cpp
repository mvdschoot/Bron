#include "Registry.h"

namespace Cheets
{

	Registry::Registry()
	{
		
	}

	Registry::~Registry()
	{

	}
	
	template<Entity T, class C, class ...Args>
	void Registry::CreateComponent(Args&& ...args)
	{
		
	}
	
	template<Entity T, class C>
	void Registry::AddComponent(C& component)
	{
		
	}
	
	template<class ...Ts>
	std::tuple<std::vector<Ts>...> Registry::Get()
	{
		
	}
	
	template<class ...Ts>
	void Registry::GroupComponents()
	{
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
			std::vector<std::type_index>::iterator internalIt = std::get<0>(groupIt).begin();
			for(; internalIt != std::get<0>(groupIt).end(); ++internalIt)
			{
				if (tuple_type == *internalIt){
					return groupIt;
				}
			}
		}
		return groupIt;
	}
}
