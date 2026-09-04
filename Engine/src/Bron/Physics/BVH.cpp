#include "BVH.h"
#include "Splitting.h"

#include <optional>

namespace bron
{
	BVH::BVH()
	{
		root = createPt<BvhNode>(AABB {{-1,-1,-1}, {1,1,1}}, nullptr, nullptr, std::vector<Pt<RigidBody>>() );
	}

	void BVH::AddObject(Pt<RigidBody> obj)
	{
		InsertPrimitive(root, obj);
	}

	std::vector<Pt<RigidBody>>& BVH::get(Pt<RigidBody>& object) 
	{
		return find(object)->primitives;
	}

	void BVH::InsertPrimitive(Pt<BvhNode> current, Pt<RigidBody> n)
	{
		BvhNode& c = *current;
		if (c.left != nullptr) {
			BR_CORE_ASSERT(c.right != nullptr, "Right child cannot be null when left child is not.");
			if (c.left->box.contains(c.box))
			{
				InsertPrimitive(c.left, n);
			}
			else if (c.right->box.contains(c.box)) {
				InsertPrimitive(c.right, n);
			}
			else {
				c.primitives.push_back(n);
			}
		} else {
			c.primitives.push_back(n);
			if (c.primitives.size() >= MAX_PRIMS_IN_NODE)
			{
				SAH::Split(current);
			}
		}
	}
	
	Pt<BvhNode> BVH::find(Pt<RigidBody> toFind) 
	{
		Pt<BvhNode> current = root;
		while (current != nullptr) /* Dummy condition. Can just as well be 'true'. */ {
			if (current->primitives.empty()) {
				if (current->left->box.contains(toFind->boundingBox)) {
					current = current->left;
				} else if (current->right->box.contains(toFind->boundingBox)) {
					current = current->right;
				} else {
					BR_CORE_ASSERT(false, "Problem with algorithm");
				}
			} else {
				BR_CORE_ASSERT(current->left == nullptr && current->right == nullptr, "Current functionality requires mutual exclusivity of left,right and primitives");
				BR_CORE_ASSERT(std::find(current->primitives.begin(), current->primitives.end(), toFind) != current->primitives.end(), "Cannot find the object");

				return current;
			}
		}
		BR_CORE_ASSERT(false, "Unreachable");
		return {};
	}
}
