#ifndef __QUEUE_HEADER__
#define __QUEUE_HEADER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "Steve/Graphics/Components/Model.h"
#include "Steve/Graphics/Components/Mesh.h"
#include "Steve/Graphics/Components/Context.h"

#include <map>

namespace Steve
{

	template<typename T, typename C>
	class SortSet
	{
	public:
		SortSet() = default;

		std::map<T*, C> pSet;
	};

	/*
	 * The meshes are already in the registry.
	 */
	class RenderQueue
	{
	public:
		RenderQueue();

		void addModel(Model* model);


		// Groups based on identical memory loc, not same attributes
		SortSet<Shader,
			SortSet<Model, 
				SortSet<Material,
					std::vector<Mesh*>>>> pQueue;
	};
}

#endif