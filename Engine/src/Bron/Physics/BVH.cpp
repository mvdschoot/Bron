#include "BVH.h"
#include "Splitting.h"

#include <optional>

namespace bron {
BVH::BVH() {
	root = CreateRef<BvhNode>(AABB{{-1, -1, -1}, {1, 1, 1}}, nullptr, nullptr, std::vector<Ref<RigidBody>>());
}

void BVH::AddObject(Ref<RigidBody> obj) { InsertPrimitive(root, obj); }

std::vector<Ref<RigidBody>>& BVH::get(Ref<RigidBody>& object) { return find(object)->primitives; }

void BVH::InsertPrimitive(Ref<BvhNode> current, Ref<RigidBody> n) {
	BvhNode& c = *current;
	if (c.left != nullptr) {
		BR_CORE_ASSERT(c.right != nullptr, "Right child cannot be null when left child is not.");
		if (c.left->box.Contains(c.box)) {
			InsertPrimitive(c.left, n);
		} else if (c.right->box.Contains(c.box)) {
			InsertPrimitive(c.right, n);
		} else {
			c.primitives.push_back(n);
		}
	} else {
		c.primitives.push_back(n);
		if (c.primitives.size() >= kMaxPrimsInNode) {
			SAH::Split(current);
		}
	}
}

Ref<BvhNode> BVH::find(Ref<RigidBody> to_find) {
	Ref<BvhNode> current = root;
	while (current != nullptr) /* Dummy condition. Can just as well be 'true'. */ {
		if (current->primitives.empty()) {
			if (current->left->box.Contains(to_find->bounding_box)) {
				current = current->left;
			} else if (current->right->box.Contains(to_find->bounding_box)) {
				current = current->right;
			} else {
				BR_CORE_ASSERT(false, "Problem with algorithm");
			}
		} else {
			BR_CORE_ASSERT(current->left == nullptr && current->right == nullptr,
						   "Current functionality requires mutual exclusivity of left,right and primitives");
			BR_CORE_ASSERT(std::find(current->primitives.begin(), current->primitives.end(), to_find) !=
								   current->primitives.end(),
						   "Cannot find the object");

			return current;
		}
	}
	BR_CORE_ASSERT(false, "Unreachable");
	return {};
}
} // namespace bron
