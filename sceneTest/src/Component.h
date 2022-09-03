#ifndef __CORE_COMPONENT_HEADER__
#define __CORE_COMPONENT_HEADER__

#include "UUID.h"

namespace Cheets
{
	struct Component 
	{
		public:
			UUID p_Id;
			bool p_IsEmpty = false;
		protected:
			Component() {}
	};

	struct EmptyComponent : Component {
		EmptyComponent() { p_IsEmpty = true; }
	};
}

#endif

