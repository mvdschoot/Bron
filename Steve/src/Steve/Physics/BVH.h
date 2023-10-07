#ifndef __BVH_HEADER__
#define __BVH_HEADER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Profiling.h"
#include "Steve/Core/Logger.h"

#include "Steve/ECS/Entity.h"

#include "PhysicsComponents.h"

#include <vector>

#include "glm/glm.hpp"
#include "Steve/Scene/Node.h"

namespace Steve
{
	struct PhysicsData;

	class BVH
	{
	private:
		static constexpr u8 max_prims_in_node = 5;

	public:

		BVH(PhysicsData* data_);

		void AddNode(Node* node);
		std::tuple<CollisionBody*, AABB> GenerateAABB(Node* node);

	private:
		void InsertPrimitive(CollisionNode* current, CollisionNode* n);
		PhysicsData* data;

	};
}

#endif