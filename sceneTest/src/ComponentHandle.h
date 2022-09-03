#ifndef __HANDLE_HEADER__ 
#define __HANDLE_HEADER__ 

#include "Component.h"

#include <typeindex>
#include <typeinfo>

namespace Cheets
{

	class IComponentHandle {
		public:
			friend bool operator<(const IComponentHandle& l,const IComponentHandle& r)
			{
				return l.m_Component->p_Id < r.m_Component->p_Id;
			}

			std::type_index p_Type;
			IComponentHandle() : p_Type(std::type_index(typeid(this))) {};
			virtual void Move(void* location) {};
			virtual size_t Size() { return 0; };
		protected:
			Component* m_Component;
	};

	template<class T>
	class ComponentHandle : public IComponentHandle
	{
		public:
			T& operator->(){
				return *(T*)m_Component;
			}
		
			T& operator*(){
				return *(T*)m_Component;
			}

			~ComponentHandle()
			{
			}

			ComponentHandle()
			{
				p_Type = std::type_index(typeid(T));
			}

            ComponentHandle(T& e) : ComponentHandle()
            {
                m_Component = &e;
            }


			ComponentHandle(IComponentHandle e) : ComponentHandle()
			{
				// CORE_ASSERT(std::type_index(typeid(*this)) == e.p_Type);
				m_Component = new T(e);
			}

			ComponentHandle(void* location, T e) : ComponentHandle()
			{
				m_Component = new(location) T(e);
			}

			template<class ...Args>
			ComponentHandle(void* location, Args&& ...args) : ComponentHandle()
			{
				m_Component = new(location) T(std::forward<Args>(args)...);
			}

			T* Get() 
			{
				return (T*)m_Component;
			}

			virtual void Move(void* location)
			{
				T* tmp = new(location) T(*(T*)m_Component);
				m_Component = (Component*)tmp;
			}

			virtual size_t Size(){
				return sizeof(T);
			}
	};

	template<class I>
	struct RemoveComponentHandle
	{
		using type = I;
	};

	template<class I>
	struct RemoveComponentHandle<ComponentHandle<I>>
	{
		using type = typename RemoveComponentHandle<I>::type;
	};
}


#endif
