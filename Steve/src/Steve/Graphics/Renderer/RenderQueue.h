#ifndef __QUEUE_HEADER__
#define __QUEUE_HEADER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"

#include "Steve/Graphics/Components/Model.h"
#include "Steve/Graphics/Components/Mesh.h"

#include <map>

#include "Steve/Graphics/MaterialBase.h"

namespace Steve
{

	/*
	 * The meshes are already in the registry.
	 */
	class RenderQueue
	{
	public:
		RenderQueue();

		void addModel(Ref<Model> model);

		std::map<const char*,
			std::map<Ref<MaterialBase>,
				std::vector<Ref<Mesh>>>> pQueue;
	};
}

#endif