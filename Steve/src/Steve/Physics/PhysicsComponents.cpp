#include "PhysicsComponents.h"

namespace Steve
{
	AABB::AABB(glm::vec3 A, glm::vec3 B)
	{
		Min = glm::min(A, B);
		Max = glm::max(A, B);
	}

	bool AABB::Contains(AABB& other)
	{
		return (other.Min.x >= Min.x && other.Max.x <= Max.x &&
			other.Min.y >= Min.y && other.Max.y <= Max.y &&
			other.Min.z >= Min.z && other.Max.z <= Max.z);
	}

	void AABB::ResizeTo(AABB& other)
	{
		Min = glm::min(Min, other.Min);
		Max = glm::max(Max, other.Max);
	}
}
