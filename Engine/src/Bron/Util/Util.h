#pragma once

#include <glm/glm.hpp>
#include <string>

#include "Bron/Core/Core.h"

namespace bron {
bool CompareFloat(float x, float y, float epsilon = 0.01f);
bool CompareFloats(glm::vec3 a, glm::vec3 b, float epsilon = 0.01f);

bool CompareFloatBits(float x, float y);
bool CompareFloatsBits(const glm::vec3& a, const glm::vec3& b);
bool CompareFloatsBits(glm::vec3* a, glm::vec3* b);
bool CompareFloatsBits(glm::vec4* a, glm::vec4* b);

std::string PrintMatrix(glm::mat4& matrix);

std::tuple<glm::vec3*, glm::vec3*, uint32_t*, u32, u32> GenSphereSmoothVertices(glm::vec3 position, float radius,
																				u32 accuracy);
} // namespace bron
