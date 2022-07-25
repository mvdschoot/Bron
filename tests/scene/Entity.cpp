#include "Entity.h"

namespace Cheets {
	Entity::Entity() {}

	Entity::~Entity() {}

	void Entity::AddedComponent(IComponentHandle* handle) {
		for (std::set<IComponentHandle*>::iterator it = m_Components.begin(); it != m_Components.end(); ++it)
		{
			if (handle->p_Type == (*it)->p_Type)
			{
				m_Components.erase(it);
			}
		}
		m_Components.insert(handle);
	}

	template <unsigned int I, class Tuple, class Components>
	void Entity::HelperStruct<I, Tuple, Components>::GetCompSpecialization(Tuple& tup, Components& components) 
	{
		using Component = std::tuple_element_t<I, Tuple>;
		std::type_index index(typeid(Component));
		std::set<Cheets::IComponentHandle>::iterator it = components.begin();
		for (; it != components.end(); ++it) {
			if (it->p_Type == index) {
				std::get<I>(tup) = *((Component*)(&(*it)));
				break;
			}
		}
		if (it == components.end()) {
			Component temp;
			temp.p_IsEmpty = true;
			std::get<I>(tup) = temp;
		}

		HelperStruct<I-1, Tuple, Components>::GetCompSpecialization(tup, components);
	}


	template<class Tuple, class Components>
	void Entity::HelperStruct<0, Tuple, Components>::GetCompSpecialization(Tuple& tup, Components& components) 
	{
		return;
	}

	template <class... Args>
	std::tuple<Args...> Entity::GetComponents() {
		if (sizeof...(Args) == 0)
			exit(0);

		std::tuple<Args...> res;
		HelperStruct<sizeof...(Args)-1, decltype(res), decltype(m_Components)>::GetCompSpecialization(res, m_Components);
		return res;
	}
}  // namespace Cheets
