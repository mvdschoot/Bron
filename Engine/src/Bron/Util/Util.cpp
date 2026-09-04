#include "Util.h"

#include <cmath>

#include "Bron/Core/Core.h"
#include "glm/ext/scalar_constants.hpp"

#include <algorithm>

namespace bron {
bool CompareFloat(float x, float y, float epsilon) {
	if (fabs(x - y) < epsilon)
		return true; // they are same
	return false; // they are not same
}

bool CompareFloats(glm::vec3 a, glm::vec3 b, float epsilon) {
	return CompareFloat(a.x, b.x, epsilon) && CompareFloat(a.y, b.y, epsilon) && CompareFloat(a.z, b.z, epsilon);
}

bool CompareFloatBits(float x, float y) { return *reinterpret_cast<u32*>(&x) == *reinterpret_cast<u32*>(&y); }

bool CompareFloatsBits(const glm::vec3& a, const glm::vec3& b) {
	return CompareFloatBits(a.x, b.x) && CompareFloatBits(a.y, b.y) && CompareFloatBits(a.z, b.z);
}

bool CompareFloatsBits(glm::vec3* a, glm::vec3* b) {
	return CompareFloatBits(a->x, b->x) && CompareFloatBits(a->y, b->y) && CompareFloatBits(a->z, b->z);
}

bool CompareFloatsBits(glm::vec4* a, glm::vec4* b) {
	return CompareFloatBits(a->x, b->x) && CompareFloatBits(a->y, b->y) && CompareFloatBits(a->z, b->z) &&
		   CompareFloatBits(a->w, b->w);
}

std::string PrintMatrix(glm::mat4& matrix) {
	return "" + std::to_string(matrix[0][0]) + "\t" + std::to_string(matrix[1][0]) + " \t" +
		   std::to_string(matrix[2][0]) + "\t" + std::to_string(matrix[3][0]) + "\n" + std::to_string(matrix[0][1]) +
		   "\t" + std::to_string(matrix[1][1]) + " \t" + std::to_string(matrix[2][1]) + "\t" +
		   std::to_string(matrix[3][1]) + "\n" + std::to_string(matrix[0][2]) + "\t" + std::to_string(matrix[1][2]) +
		   " \t" + std::to_string(matrix[2][2]) + "\t" + std::to_string(matrix[3][2]) + "\n" +
		   std::to_string(matrix[0][3]) + "\t" + std::to_string(matrix[1][3]) + " \t" + std::to_string(matrix[2][3]) +
		   "\t" + std::to_string(matrix[3][3]) + "\n\n";
}

std::tuple<glm::vec3*, glm::vec3*, uint32_t*, u32, u32> GenSphereSmoothVertices(glm::vec3 position, float radius,
																				u32 accuracy) {
	const int num_slices = accuracy;
	const int num_stacks = accuracy / 2;

	int num_vertices = (num_slices + 1) * (num_stacks + 1);
	int num_indices = 6 * num_slices * num_stacks;

	u8* storage = new u8[sizeof(glm::vec3) * num_vertices * 2 + sizeof(u32) * num_indices];
	glm::vec3* vertices = (glm::vec3*) storage;
	glm::vec3* normals = (glm::vec3*) (storage + sizeof(glm::vec3) * num_vertices);
	u32* indices = (u32*) (storage + 2 * sizeof(glm::vec3) * num_vertices);

	float d_theta = 2.0f * glm::pi<float>() / num_slices;
	float d_phi = glm::pi<float>() / num_stacks;

	int vertex_index = 0;
	int index_index = 0;

	// Generate vertices and normals
	for (int stack = 0; stack <= num_stacks; stack++) {
		float phi = stack * d_phi;
		for (int slice = 0; slice <= num_slices; slice++) {
			float theta = slice * d_theta;

			float x = radius * sin(phi) * cos(theta);
			float y = radius * cos(phi);
			float z = radius * sin(phi) * sin(theta);

			vertices[vertex_index] = glm::vec3(x, y, z);
			normals[vertex_index] = glm::normalize(vertices[vertex_index] - position);
			vertex_index++;
		}
	}

	// Generate indices
	for (int stack = 0; stack < num_stacks; stack++) {
		for (int slice = 0; slice < num_slices; slice++) {
			int v1 = stack * (num_slices + 1) + slice;
			int v2 = v1 + 1;
			int v3 = (stack + 1) * (num_slices + 1) + slice;
			int v4 = v3 + 1;

			indices[index_index++] = v1;
			indices[index_index++] = v2;
			indices[index_index++] = v3;

			indices[index_index++] = v2;
			indices[index_index++] = v4;
			indices[index_index++] = v3;
		}
	}

	return std::make_tuple(vertices, normals, indices, num_vertices, num_indices);
}
std::string ToLowerCase(const std::string& str) {
	std::string result(str);
	std::ranges::transform(result, result.begin(), tolower);
	return result;
}

} // namespace bron
