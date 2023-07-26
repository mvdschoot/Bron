#ifndef __BVH_HEADER__
#define __BVH_HEADER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Profiling.h"
#include "Steve/Core/Logger.h"

#include "Steve/ECS/Entity.h"

#include "PhysicsComponents.h"

#include <vector>

#include "glm/glm.hpp"

namespace Steve
{
	class BVH
	{
	private:
		static constexpr u8 max_prims_in_node = 5;

	public:

		BVH();

		void AddNode(Entity* entity);
		std::tuple<CollisionBody*, AABB> GenerateAABB(Entity* entity);

	private:
		void InsertPrimitive(CollisionNod* current, CollisionNod* n);

		std::vector<CollisionNod> Nodes;

	};
}

#endif