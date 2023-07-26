#ifndef _BVHSPLITTING_HEADER__
#define _BVHSPLITTING_HEADER__

#include "PhysicsComponents.h"

namespace Steve
{
	class SAH
	{
	public:
		static void Split(std::vector<CollisionNod>& nodes, CollisionNod& node);

	private:
		static float Cost(CollisionNod& node, AABB a, AABB b);

		static constexpr u8 SplitPerAxis = 3;
		static constexpr u8 CostTraverse = 1;
		static constexpr u8 CostIntersect = 2;
	};
}

#endif