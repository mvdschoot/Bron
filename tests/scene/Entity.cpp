#include "Entity.h"

namespace Cheets
{
	Entity::Entity(std::string name)
	{
		p_Name = name;
	}
	
	Entity::~Entity(){}

	void Entity::AddedComponent(IComponentHandle* handle)
	{
		m_Components.insert(handle);
	}

	template<class ...Args>
	std::tuple<Args&...> Entity::GetComponents()
	{
		std::tuple<Args&...> res;
		GetCompSpecialization<1, declspec(res), Args...>(res);
	}

	template<std::size_t I, class Tuple, class First>
	void Entity::GetCompSpecialization(Tuple& tup)
	{
		std::type_index index(typeid(First));
		std::set<IComponentHandle*>::iterator it = m_Components.begin();
		for (; it != m_Components.end(); ++it)
		{
			if ((*it)->m_Type == index){
				std::get<0>(tup) = *((First*)(*it));
				break;
			}
		}
		if(it == m_Components.end()) {
			First temp;
			std::get<0>(tup) = temp;
		}
	}
	
	template <std::size_t I, class Tuple, class First, class Second, class ...Rest>
	void Entity::GetCompSpecialization(Tuple& tup)
	{
		std::type_index index(typeid(Second));
		std::set<IComponentHandle*>::iterator it = m_Components.begin();
		for (; it != m_Components.end(); ++it)
		{
			if ((*it)->m_Type == index){
				std::get<I>(tup) = *((Second*)(*it));
				break;
			}
		}
		if(it == m_Components.end()) {
			Second temp;
			std::get<I>(tup) = temp;
		}
		GetCompSpecialization<++I, Tuple, First, Rest...>(tup);
	}
}
