#pragma once

#include "Components.h"
#include "BVH.h"

namespace bron
{
	class SAH
	{
	public:
		static void Split(Ref<BvhNode> node);

	private:
		static float Cost(Ref<BvhNode> node, AABB a, AABB b);

		static constexpr u8 SplitPerAxis = 3;
		static constexpr u8 CostTraverse = 1;
		static constexpr u8 CostIntersect = 2;
	};
}
