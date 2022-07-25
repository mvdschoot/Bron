#ifndef __HANDLE_HEADER__ 
#define __HANDLE_HEADER__ 

#include "Component.h"

#include <typeindex>
#include <typeinfo>

namespace Cheets
{
	class IComponentHandle {
		public:
			std::type_index p_Type;
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

			ComponentHandle(void* location, T e);
			
			template<class ...Args>
			ComponentHandle(void* location, Args&& ...args);

			void Move(void* location);
	};

}

#endif
