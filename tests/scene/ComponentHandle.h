#ifndef __HANDLE_HEADER__ 
#define __HANDLE_HEADER__ 

#include "Cheets/Core/Core.h"
#include "Cheets/Core/Logger.h"
#include "Cheets/Core/Profiling.h"

#include "Components.h"

#include <typeindex>
#include <typeinfo>

namespace Cheets
{
	class IComponentHandle {
		protected:
			Component* m_Component;
			std::type_index m_Type;
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
