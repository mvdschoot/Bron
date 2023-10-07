#include "BVH.h"

#include "Splitting.h"

#include "PhysicsEngine.h"

namespace Steve
{
	BVH::BVH(PhysicsData* data_)
		: data(data_)
	{
		data->Nodes.push_back({ {{-1,-1,-1}, {1,1,1}}, nullptr, nullptr, nullptr });
	}

	void BVH::AddNode(Node* node)
	{
		auto [body, box] = GenerateAABB(node);
		
		data->Nodes.push_back({ box, body, nullptr, nullptr });
		InsertPrimitive(data->Nodes.data(), &data->Nodes.back());
	}

	std::tuple<CollisionBody*, AABB> BVH::GenerateAABB(Node* node)
	{
		if(node->Contains<SphereCollisionBody>())
		{
			SphereCollisionBody& col = node->GetComponent<SphereCollisionBody>();
			return std::make_tuple(&col, AABB(col.Transform->Position - col.Radius, col.Transform->Position + col.Radius));
		}
		if (node->Contains<CubeCollisionBody>())
		{
			CubeCollisionBody& s = node->GetComponent<CubeCollisionBody>();
			return std::make_tuple(&s, AABB{ s.Transform->Position - (s.Dimensions * 0.5f), s.Transform->Position + (s.Dimensions * 0.5f)});
		}

		CORE_ASSERT(false, "No collision body detected")
	}

	void BVH::InsertPrimitive(CollisionNode* current, CollisionNode* n)
	{
		CollisionNode& c = *current;
		if (c.Left != nullptr) {
			if (c.Left->Box.Contains(c.Box))
			{
				InsertPrimitive(c.Left, n);
			}
			else if (c.Right->Box.Contains(c.Box)) {
				InsertPrimitive(c.Right, n);
			}
			else {
				c.Primitives.push_back(n);
			}
			return;
		}

		c.Primitives.push_back(n);
		if (c.Primitives.size() >= max_prims_in_node)
		{
			SAH::Split(data->Nodes, c);
		}
	}
	
}
