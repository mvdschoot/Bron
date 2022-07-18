#ifndef __SCENE_HEADER__
#define __SCENE_HEADER__

#include "Cheets/Core/Core.h"
#include "Cheets/Core/Logger.h"
#include "Cheets/Core/Profiling.h"

#include "Registry.h"

namespace Cheets
{
	class Scene
	{
		public:
			Scene();
			~Scene();


		private:
			Registry m_Registery;
	};
}


#endif
