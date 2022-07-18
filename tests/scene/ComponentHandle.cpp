#include "ComponentHandle.h"

namespace Cheets
{
	template<class T>
	ComponentHandle<T>::ComponentHandle(void* location, T e)
	{
		m_Component = new(location) T(e);
		m_Type = std::type_index(typeid(T));
	}

	
	template<class T>
	template<class ...Args>
	ComponentHandle<T>::ComponentHandle(void* location, Args&& ...args)
	{
		m_Component = new(location) T(std::forward<Args>(args)...);
		m_Type = std::type_index(typeid(T));
	}

	template<class T>
	void ComponentHandle<T>::Move(void* location)
	{
		m_Component = new(location) T(*(T*)m_Component);
	}
	
}
