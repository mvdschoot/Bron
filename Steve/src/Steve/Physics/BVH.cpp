#include "BVH.h"

#include "Splitting.h"

namespace Steve
{
	BVH::BVH()
	{
		Nodes.push_back({ {{-1,-1,-1}, {1,1,1}}, nullptr, nullptr, nullptr });
	}

	void BVH::AddNode(Entity* entity)
	{
		auto [body, box] = GenerateAABB(entity);
		
		Nodes.push_back({ box, body, nullptr, nullptr });
		InsertPrimitive(Nodes.data(), &Nodes.back());
	}

	std::tuple<CollisionBody*, AABB> BVH::GenerateAABB(Entity* entity)
	{
		if(entity->Contains<SphereCollisionBody>())
		{
			SphereCollisionBody& col = entity->GetComponent<SphereCollisionBody>();
			glm::vec3 pos = std::get<0>(col.Transform->Get());
			return std::make_tuple(&col, AABB(pos - col.Radius, pos + col.Radius));
		}
		if (entity->Contains<CubeCollisionBody>())
		{
			CubeCollisionBody& s = entity->GetComponent<CubeCollisionBody>();
			glm::vec3 pos = std::get<0>(s.Transform->Get());
			return std::make_tuple(&s, AABB{ pos - (s.Dimensions * 0.5f), pos + (s.Dimensions * 0.5f)});
		}

		CORE_ASSERT(false, "No collision body detected")
	}

	void BVH::InsertPrimitive(Node* current, Node* n)
	{
		Node& c = *current;
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
			SAH::Split(Nodes, c);
		}
	}
	
}
