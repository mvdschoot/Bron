#pragma once

#include "Components.h"
#include "BVH.h"

namespace Bron
{
	class SAH
	{
	public:
		static void Split(Pt<BvhNode> node);

	private:
		static float Cost(Pt<BvhNode> node, AABB a, AABB b);

		static constexpr u8 SplitPerAxis = 3;
		static constexpr u8 CostTraverse = 1;
		static constexpr u8 CostIntersect = 2;
	};
}
