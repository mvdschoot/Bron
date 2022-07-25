#ifndef __SCENE_HEADER__
#define __SCENE_HEADER__

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
