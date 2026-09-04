#pragma once

#include "Defs.h"

#include <glm/glm.hpp>

#include <algorithm>

namespace bron
{
	using vec3 = glm::vec3;

	inline vec3 AddScaledVector(vec3& a, vec3& b, float scaling)
	{
		return a + (b * scaling);
	}
}
