#ifndef ___UTIL_HEADER___
#define ___UTIL_HEADER___

#include <glm/glm.hpp>

namespace Steve
{
    bool compare_float(float x, float y, float epsilon = 0.01f);
    bool compare_floats(glm::vec3 a, glm::vec3 b, float epsilon = 0.01f);

    bool compare_float_bits(float x, float y);
    bool compare_floats_bits(glm::vec3 a, glm::vec3 b);
}


#endif