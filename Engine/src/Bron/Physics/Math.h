#pragma once

#include "Defs.h"

#include <glm/glm.hpp>

#include <algorithm>

namespace Bron
{
	using vec3 = glm::vec3;

	vec3 addScaledVector(vec3& a, vec3& b, float scaling)
	{
		return a + (b * scaling);
	}
}
