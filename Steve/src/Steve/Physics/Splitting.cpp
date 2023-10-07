#include "Splitting.h"

#include <limits>

namespace Steve
{
	void SAH::Split(std::vector<CollisionNode>& nodes, CollisionNode& node)
	{
		const float splitStepX = (node.Box.Max.x - node.Box.Min.x) / (float)(SplitPerAxis + 1);
		const float splitStepY = (node.Box.Max.y - node.Box.Min.y) / (float)(SplitPerAxis + 1);
		const float splitStepZ = (node.Box.Max.z - node.Box.Min.z) / (float)(SplitPerAxis + 1);

		float minCost = std::numeric_limits<float>::max();
		glm::vec3 minSplit(1.0f);

		for (u8 x = 1; x < SplitPerAxis + 1; x++)
		{
			glm::vec3 s = node.Box.Min + glm::vec3(x * splitStepX, 0, 0);
			float cost = Cost(node, AABB(node.Box.Min, s), AABB(s, node.Box.Max));
			if (cost < minCost)
			{
				minSplit = s;
				minCost = cost;
			}
		}
		for (u8 x = 1; x < SplitPerAxis + 1; x++)
		{
			glm::vec3 s = node.Box.Min + glm::vec3(0, x * splitStepY, 0);
			float cost = Cost(node, AABB(node.Box.Min, s), AABB(s, node.Box.Max));
			if (cost < minCost)
			{
				minSplit = s;
				minCost = cost;
			}
		}
		for (u8 x = 1; x < SplitPerAxis + 1; x++)
		{
			glm::vec3 s = node.Box.Min + glm::vec3(0, 0, x * splitStepZ);
			float cost = Cost(node, AABB(node.Box.Min, s), AABB(s, node.Box.Max));
			if (cost < minCost)
			{
				minSplit = s;
				minCost = cost;
			}
		}

		AABB a = AABB(node.Box.Min, minSplit), b = AABB(minSplit, node.Box.Max);
		nodes.push_back({ a, nullptr, nullptr, nullptr });
		nodes.push_back({ b, nullptr, nullptr, nullptr });

		node.Left = &nodes[nodes.size() - 2];
		node.Right = &nodes[nodes.size() - 1];

		for(int x = 0; x < node.Primitives.size(); x++)
		{
			if (a.Contains(node.Primitives[x]->Box))
			{
				node.Left->Primitives.push_back(node.Primitives[x]);
				node.Primitives.erase(node.Primitives.begin() + x);
				x--;
			} else if (b.Contains(node.Primitives[x]->Box))
			{
				node.Right->Primitives.push_back(node.Primitives[x]);
				node.Primitives.erase(node.Primitives.begin() + x);
				x--;
			}
		}
	}


	float SAH::Cost(CollisionNode& node, AABB a, AABB b)
	{
		glm::vec3 box = node.Box.Max - node.Box.Min;
		float volume = box.x * box.y * box.z;

		box = a.Max - a.Min;
		float Asa = volume / (box.x * box.y * box.z);

		box = b.Max - b.Min;
		float Bsa = volume / (box.x * box.y * box.z);

		int l = 0, r = 0;
		for(CollisionNode* n : node.Primitives)
		{
			if (a.Contains(n->Box))
			{
				l++;
			} else if (b.Contains(n->Box))
			{
				r++;
			}
		}

		return CostTraverse + (Asa * 2 * l) + (Bsa * 2 * r);
	}
}
