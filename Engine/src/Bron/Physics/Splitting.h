#pragma once

#include "Components.h"
#include "BVH.h"

namespace bron {
class SAH {
public:
	static void Split(Ref<BvhNode> node);

private:
	static float Cost(Ref<BvhNode> node, AABB a, AABB b);

	static constexpr u8 kSplitPerAxis = 3;
	static constexpr u8 kCostTraverse = 1;
	static constexpr u8 kCostIntersect = 2;
};
} // namespace bron
