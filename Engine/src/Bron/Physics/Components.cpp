#include "Components.h"

namespace bron
{
	AABB::AABB(vec3 A, vec3 B)
	{
		min = glm::min(A, B);
		max = glm::max(A, B);
	}

	bool AABB::Contains(AABB& other)
	{
		return (other.min.x >= min.x && other.max.x <= max.x &&
			other.min.y >= min.y && other.max.y <= max.y &&
			other.min.z >= min.z && other.max.z <= max.z);
	}

	void AABB::ResizeTo(AABB& to)
	{
		min = glm::min(min, to.min);
		max = glm::max(max, to.max);
	}
}
