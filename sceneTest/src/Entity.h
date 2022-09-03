#ifndef __ENTITY_HEADER__
#define __ENTITY_HEADER__

#include "Core.h"
#include "UUID.h"
#include "ComponentHandle.h"

#include <set>
#include <map>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <utility>

#include <iostream>

namespace Cheets
{

	class Entity
	{
		friend class Registry;

		public:
			Entity();
			~Entity();

			bool operator==(const Entity& other){return p_Id == other.p_Id;}

			template <class... Args>
			std::tuple<ComponentHandle<Args>*...> GetComponents() 
			{
				// CORE_ASSERT(sizeof...(Args) > 0);

				std::tuple<ComponentHandle<Args>*...> res;
				GetCompSpec(res, std::make_integer_sequence<size_t, sizeof...(Args)>());
				return res;
			}

			template<class Component>
			constexpr ComponentHandle<Component>* GetComponent()
			{
				const std::type_index index(typeid(Component));
				auto it = m_Components.begin();
				for (; it != m_Components.end(); ++it) {
					if ((*it)->p_Type == index) {
						return static_cast<ComponentHandle<Component>*>(it->get());
					}
				}
				throw std::invalid_argument("No component of this type found");
			}

			template<class Tuple, std::size_t ...I>
			void GetCompSpec(Tuple& tup, std::index_sequence<I...>)
			{
				([&, this]() {
					typedef RemoveComponentHandle<std::remove_pointer_t<std::tuple_element_t<I, Tuple>>>::type Type;
					std::get<I>(tup) = this->GetComponent<Type>();
				}(), ...);
			}

			void AddedComponent(const Ref<IComponentHandle> handle) {
				for (auto it = m_Components.begin(); it != m_Components.end(); ++it)
				{
					if (handle->p_Type == (*it)->p_Type)
					{
						//Swap mechanism in storage
						m_Components.erase(it);
					}
				}
				m_Components.insert(handle);
			}

		public:
			UUID p_Id;
		private:
			std::set<Ref<IComponentHandle>> m_Components;

	};
}


#endif

