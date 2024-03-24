#ifndef __PHYSICS_MATH_HEADER__
#define __PHYSICS_MATH_HEADER__

#include "Defs.h"

#include <glm/glm.hpp>

#include <algorithm>

namespace Steve
{
	using vec3 = glm::vec3;

	vec3 addScaledVector(vec3& a, vec3& b, float scaling)
	{
		return a + (b * scaling);
	}
}

#endif // __PHYSICS_MATH_HEADER__