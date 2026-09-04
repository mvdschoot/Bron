#include "Splitting.h"

#include <limits>

namespace bron
{
	void SAH::Split(Ref<BvhNode> node)
	{
		const float split_step_x = (node->box.max.x - node->box.min.x) / (float)(SplitPerAxis + 1);
		const float split_step_y = (node->box.max.y - node->box.min.y) / (float)(SplitPerAxis + 1);
		const float split_step_z = (node->box.max.z - node->box.min.z) / (float)(SplitPerAxis + 1);

		float min_cost = std::numeric_limits<float>::max();
		glm::vec3 min_split(1.0f);

		for (u8 x = 1; x < SplitPerAxis + 1; x++)
		{
			glm::vec3 s = node->box.min + glm::vec3(x * split_step_x, 0, 0);
			float cost = Cost(node, AABB(node->box.min, s), AABB(s, node->box.max));
			if (cost < min_cost)
			{
				min_split = s;
				min_cost = cost;
			}
		}
		for (u8 x = 1; x < SplitPerAxis + 1; x++)
		{
			glm::vec3 s = node->box.min + glm::vec3(0, x * split_step_y, 0);
			float cost = Cost(node, AABB(node->box.min, s), AABB(s, node->box.max));
			if (cost < min_cost)
			{
				min_split = s;
				min_cost = cost;
			}
		}
		for (u8 x = 1; x < SplitPerAxis + 1; x++)
		{
			glm::vec3 s = node->box.min + glm::vec3(0, 0, x * split_step_z);
			float cost = Cost(node, AABB(node->box.min, s), AABB(s, node->box.max));
			if (cost < min_cost)
			{
				min_split = s;
				min_cost = cost;
			}
		}

		AABB a = AABB(node->box.min, min_split);
		AABB b = AABB(min_split, node->box.max);
		
		node->left = CreateRef<BvhNode>(BvhNode {a, nullptr, nullptr});
		node->right = CreateRef<BvhNode>(BvhNode {a, nullptr, nullptr});
		
		for(int x = 0; x < node->primitives.size(); x++)
		{
			if (a.Contains(node->primitives[x]->bounding_box))
			{
				node->left->primitives.push_back(node->primitives[x]);
				node->primitives.erase(node->primitives.begin() + x);
				x--;
			} else if (b.Contains(node->primitives[x]->bounding_box))
			{
				node->right->primitives.push_back(node->primitives[x]);
				node->primitives.erase(node->primitives.begin() + x);
				x--;
			} else {
				BR_CORE_ASSERT(false, "Node does not fit in either box, algorithm is faulty.");
			}
		}
	}


	float SAH::Cost(Ref<BvhNode> node, AABB a, AABB b)
	{
		glm::vec3 box = node->box.max - node->box.min;
		float volume = box.x * box.y * box.z;

		box = a.max - a.min;
		float Asa = volume / (box.x * box.y * box.z);

		box = b.max - b.min;
		float Bsa = volume / (box.x * box.y * box.z);

		int l = 0, r = 0;
		for(Ref<RigidBody>& n : node->primitives)
		{
			if (a.Contains(n->bounding_box)) {
				l++;
			} else if (b.Contains(n->bounding_box)) {
				r++;
			} else {
				BR_CORE_ASSERT(false, "Node does not fit in either box, algorithm is faulty.");
			}
		}

		return CostTraverse + (Asa * 2 * l) + (Bsa * 2 * r);
	}
}
