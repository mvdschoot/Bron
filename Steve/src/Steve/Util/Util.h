#ifndef ___UTIL_HEADER___
#define ___UTIL_HEADER___

#include <glm/glm.hpp>
#include <string>

#include "Steve/Core/Core.h"

namespace Steve
{
    bool compare_float(float x, float y, float epsilon = 0.01f);
    bool compare_floats(glm::vec3 a, glm::vec3 b, float epsilon = 0.01f);

    bool compare_float_bits(float x, float y);
    bool compare_floats_bits(const glm::vec3& a, const glm::vec3& b);
    bool compare_floats_bits(glm::vec3* a, glm::vec3* b);
    bool compare_floats_bits(glm::vec4* a, glm::vec4* b);

    std::string print_matrix(glm::mat4& matrix);

	std::tuple<glm::vec3*, glm::vec3*, uint32_t*, u32, u32> GenSphereSmoothVertices(glm::vec3 position, float radius, u32 accuracy);
}


#endif