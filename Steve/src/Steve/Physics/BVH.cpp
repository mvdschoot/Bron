#include "BVH.h"
#include "Splitting.h"

#include <optional>

namespace Steve
{
	BVH::BVH()
	{
		root = createPt<BvhNode>(AABB {{-1,-1,-1}, {1,1,1}}, nullptr, nullptr, std::vector<Pt<RigidBody>>() );
	}

	void BVH::addObject(Pt<RigidBody> obj)
	{
		insertPrimitive(root, obj);
	}

	std::vector<Pt<RigidBody>>& BVH::get(Pt<RigidBody>& object) 
	{
		return find(object)->primitives;
	}

	void BVH::insertPrimitive(Pt<BvhNode> current, Pt<RigidBody> n)
	{
		BvhNode& c = *current;
		if (c.left != nullptr) {
			CORE_ASSERT(c.right != nullptr, "Right child cannot be null when left child is not.");
			if (c.left->box.contains(c.box))
			{
				insertPrimitive(c.left, n);
			}
			else if (c.right->box.contains(c.box)) {
				insertPrimitive(c.right, n);
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
					CORE_ASSERT(false, "Problem with algorithm");
				}
			} else {
				CORE_ASSERT(current->left == nullptr && current->right == nullptr, "Current functionality requires mutual exclusivity of left,right and primitives");
				CORE_ASSERT(std::find(current->primitives.begin(), current->primitives.end(), toFind) != current->primitives.end(), "Cannot find the object");

				return current;
			}
		}
		CORE_ASSERT(false, "Unreachable");
		return {};
	}
}
