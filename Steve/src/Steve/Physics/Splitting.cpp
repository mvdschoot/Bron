#include "Splitting.h"

#include <limits>

namespace Steve
{
	void SAH::Split(Pt<BvhNode> node)
	{
		const float splitStepX = (node->box.max.x - node->box.min.x) / (float)(SplitPerAxis + 1);
		const float splitStepY = (node->box.max.y - node->box.min.y) / (float)(SplitPerAxis + 1);
		const float splitStepZ = (node->box.max.z - node->box.min.z) / (float)(SplitPerAxis + 1);

		float minCost = std::numeric_limits<float>::max();
		glm::vec3 minSplit(1.0f);

		for (u8 x = 1; x < SplitPerAxis + 1; x++)
		{
			glm::vec3 s = node->box.min + glm::vec3(x * splitStepX, 0, 0);
			float cost = Cost(node, AABB(node->box.min, s), AABB(s, node->box.max));
			if (cost < minCost)
			{
				minSplit = s;
				minCost = cost;
			}
		}
		for (u8 x = 1; x < SplitPerAxis + 1; x++)
		{
			glm::vec3 s = node->box.min + glm::vec3(0, x * splitStepY, 0);
			float cost = Cost(node, AABB(node->box.min, s), AABB(s, node->box.max));
			if (cost < minCost)
			{
				minSplit = s;
				minCost = cost;
			}
		}
		for (u8 x = 1; x < SplitPerAxis + 1; x++)
		{
			glm::vec3 s = node->box.min + glm::vec3(0, 0, x * splitStepZ);
			float cost = Cost(node, AABB(node->box.min, s), AABB(s, node->box.max));
			if (cost < minCost)
			{
				minSplit = s;
				minCost = cost;
			}
		}

		AABB a = AABB(node->box.min, minSplit);
		AABB b = AABB(minSplit, node->box.max);
		
		node->left = createPt<BvhNode>(BvhNode {a, nullptr, nullptr});
		node->right = createPt<BvhNode>(BvhNode {a, nullptr, nullptr});
		
		for(int x = 0; x < node->primitives.size(); x++)
		{
			if (a.contains(node->primitives[x]->boundingBox))
			{
				node->left->primitives.push_back(node->primitives[x]);
				node->primitives.erase(node->primitives.begin() + x);
				x--;
			} else if (b.contains(node->primitives[x]->boundingBox))
			{
				node->right->primitives.push_back(node->primitives[x]);
				node->primitives.erase(node->primitives.begin() + x);
				x--;
			} else {
				CORE_ASSERT(false, "Node does not fit in either box, algorithm is faulty.");
			}
		}
	}


	float SAH::Cost(Pt<BvhNode> node, AABB a, AABB b)
	{
		glm::vec3 box = node->box.max - node->box.min;
		float volume = box.x * box.y * box.z;

		box = a.max - a.min;
		float Asa = volume / (box.x * box.y * box.z);

		box = b.max - b.min;
		float Bsa = volume / (box.x * box.y * box.z);

		int l = 0, r = 0;
		for(Pt<RigidBody>& n : node->primitives)
		{
			if (a.contains(n->boundingBox)) {
				l++;
			} else if (b.contains(n->boundingBox)) {
				r++;
			} else {
				CORE_ASSERT(false, "Node does not fit in either box, algorithm is faulty.");
			}
		}

		return CostTraverse + (Asa * 2 * l) + (Bsa * 2 * r);
	}
}
