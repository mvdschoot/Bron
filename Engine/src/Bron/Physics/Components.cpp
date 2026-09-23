#include "Components.h"

namespace bron {
P_AABB::P_AABB(vec3 A, vec3 b) {
	min = glm::min(A, b);
	max = glm::max(A, b);
}

bool P_AABB::Contains(P_AABB& other) {
	return (other.min.x >= min.x && other.max.x <= max.x && other.min.y >= min.y && other.max.y <= max.y &&
			other.min.z >= min.z && other.max.z <= max.z);
}

void P_AABB::ResizeTo(P_AABB& to) {
	min = glm::min(min, to.min);
	max = glm::max(max, to.max);
}
} // namespace bron
