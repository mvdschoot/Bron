#include "Util.h"

#include <cmath>

#include "Steve/Core/Core.h"

namespace Steve
{
	bool compare_float(float x, float y, float epsilon)
	{
		if (fabs(x - y) < epsilon)
			return true; //they are same
		return false; //they are not same
	}

	bool compare_floats(glm::vec3 a, glm::vec3 b, float epsilon)
	{
		return compare_float(a.x, b.x, epsilon)
			&& compare_float(a.y, b.y, epsilon)
			&& compare_float(a.z, b.z, epsilon);
	}

	bool compare_float_bits(float x, float y)
	{
		return *reinterpret_cast<u32*>(&x) == *reinterpret_cast<u32*>(&y);
	}

	bool compare_floats_bits(glm::vec3 a, glm::vec3 b)
	{
		return compare_float_bits(a.x, b.x)
			&& compare_float_bits(a.y, b.y)
			&& compare_float_bits(a.z, b.z);
	}
}
