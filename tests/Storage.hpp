#ifndef __CUSTOMSTORAGE_HEADER__
#define __CUSTOMSTORAGE_HEADER__

#include <unordered_map>
#include <vector>
#include <functional>
#include <utility>
#include <typeindex>
#include <typeinfo>

namespace Cheets
{

	class TemplateContainer
	{
		public:
			TemplateContainer()
			{
			
			}

			~TemplateContainer()
			{

				clear();

			}
			TemplateContainer(const TemplateContainer& _other)
			{
				*this = _other;
			}
		
			TemplateContainer& operator=(const TemplateContainer& _other)
			{
				clear();
				m_ClearFunctions = _other.m_ClearFunctions;
				m_CopyFunctions = _other.m_CopyFunctions;

				for (auto&& copy_function : m_CopyFunctions)
				{
					copy_function(_other, *this);
				}
				return *this;
			}

			template<class T>
			void insert(T& el)
			{
				if (m_Storage<T>.find(this) == std::end(m_Storage<T>))
				{
					m_ClearFunctions.emplace_back([](TemplateContainer& _c){m_Storage<T>.erase(&_c);});
					m_CopyFunctions.emplace_back([](const TemplateContainer& _from, TemplateContainer& _to)
            				{
				                m_Storage<T>[&_to] = m_Storage<T>[&_from];
            				});
					m_ClearTemplateByIndex.emplace(std::make_pair(std::type_index(typeid(T)), 
					[](const TemplateContainer* c) {
						m_Storage<T>.erase(c);
					}));
					
				}
				m_Storage<T>[this] = el;
			}
			
			template<class T, class ...Args>
			void create(Args... args)
			{
				insert(T(std::forward<Args>(args)...));
			}


			void clear()
		    {
				for (auto&& clear_func : m_ClearFunctions)
				{
				    clear_func(*this);
				}
		    }

			template<class T>
			void clear()
			{
				m_Storage<T>.erase(this);
			}

			void clear(std::type_index type)
			{
				m_ClearTemplateByIndex[type](this);
			}

			template<class T>
			T& get()
			{
				return m_Storage<T>[this];
			}

			template<class T>
			void visit(T&& visitor)
			{
				visit_impl(visitor, typename std::decay_t<T>::types{});
			}

			template<class T, template<class...> class TLIST, class... TYPES>
			void visit_impl(T&& visitor, TLIST<TYPES...>)
			{
				(..., visit_impl_help<std::decay_t<T>, TYPES>(visitor));
			}

			template<class T, class U>
			void visit_impl_help(T& visitor)
			{
				for (auto&& element : m_Storage<U>[this])
				{
					visitor(element);
				}
			}

		private:
			template<class T>
			static std::unordered_map<const TemplateContainer*, T> m_Storage;

			std::unordered_map<std::type_index, std::function<void(const TemplateContainer*)>> m_ClearTemplateByIndex;
			std::vector<std::function<void(TemplateContainer&)>> m_ClearFunctions;
			std::vector<std::function<void(const TemplateContainer&, TemplateContainer&)>> m_CopyFunctions;
	};

	template<class T>
	std::unordered_map<const TemplateContainer*, T> TemplateContainer::m_Storage;

	template<class...>
	struct type_list{};

	template<class... TYPES>
	struct visitor_base
	{
		using types = type_list<TYPES...>;
	};
}

#endif
