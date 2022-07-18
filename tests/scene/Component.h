#ifndef __CORE_COMPONENT_HEADER__
#define __CORE_COMPONENT_HEADER__

#include "Cheets/Core/Core.h"
#include "Cheets/Core/Logger.h"

#include "Cheets/Utils/UUID.hpp"

namespace Cheets
{
	struct Component 
	{
		public:
			UUID Id;
			
		protected:
			Component() {}
	};
}

#endif

