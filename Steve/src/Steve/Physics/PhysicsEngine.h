#ifndef __PHYSICSENGINE_HEADER__
#define __PHYSICSENGINE_HEADER__
#include "BVH.h"

namespace Steve
{
	struct PhysicsData
	{
		BVH Bvh;

		std::vector<CollisionNode> Nodes;
	};

	class PhysicsEngine
	{
	public:
		PhysicsEngine();

		void AddNode(Node* node);

	private:
		PhysicsData data;
	};
}

#endif