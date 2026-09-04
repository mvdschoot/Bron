#include "Util.h"

#include <cmath>

#include "Bron/Core/Core.h"
#include "glm/ext/scalar_constants.hpp"

namespace bron
{
	bool CompareFloat(float x, float y, float epsilon)
	{
		if (fabs(x - y) < epsilon)
			return true; //they are same
		return false; //they are not same
	}

	bool CompareFloats(glm::vec3 a, glm::vec3 b, float epsilon)
	{
		return CompareFloat(a.x, b.x, epsilon)
			&& CompareFloat(a.y, b.y, epsilon)
			&& CompareFloat(a.z, b.z, epsilon);
	}

	bool CompareFloatBits(float x, float y)
	{
		return *reinterpret_cast<u32*>(&x) == *reinterpret_cast<u32*>(&y);
	}

	bool CompareFloatsBits(const glm::vec3 &a, const glm::vec3 &b) {
		return CompareFloatBits(a.x, b.x)
			&& CompareFloatBits(a.y, b.y)
			&& CompareFloatBits(a.z, b.z);
	}

	bool CompareFloatsBits(glm::vec3* a, glm::vec3* b)
	{
		return CompareFloatBits(a->x, b->x)
			&& CompareFloatBits(a->y, b->y)
			&& CompareFloatBits(a->z, b->z);
	}

	bool CompareFloatsBits(glm::vec4* a, glm::vec4* b)
	{
		return CompareFloatBits(a->x, b->x)
			&& CompareFloatBits(a->y, b->y)
			&& CompareFloatBits(a->z, b->z)
			&& CompareFloatBits(a->w, b->w);
	}

	std::string PrintMatrix(glm::mat4 &matrix) {
		return "" + std::to_string(matrix[0][0]) + "\t" + std::to_string(matrix[1][0]) + " \t" + std::to_string(matrix[2][0]) + "\t" + std::to_string(matrix[3][0]) + "\n" +
					std::to_string(matrix[0][1]) + "\t" + std::to_string(matrix[1][1]) + " \t" + std::to_string(matrix[2][1]) + "\t" + std::to_string(matrix[3][1]) + "\n" +
					std::to_string(matrix[0][2]) + "\t" + std::to_string(matrix[1][2]) + " \t" + std::to_string(matrix[2][2]) + "\t" + std::to_string(matrix[3][2]) + "\n" +
					std::to_string(matrix[0][3]) + "\t" + std::to_string(matrix[1][3]) + " \t" + std::to_string(matrix[2][3]) + "\t" + std::to_string(matrix[3][3]) + "\n\n";
	}

	std::tuple<glm::vec3*, glm::vec3*, uint32_t*, u32, u32> GenSphereSmoothVertices(glm::vec3 position, float radius, u32 accuracy)
	{
		const int numSlices = accuracy;
		const int numStacks = accuracy / 2;

		int numVertices = (numSlices + 1) * (numStacks + 1);
		int numIndices = 6 * numSlices * numStacks;

		u8* storage = new u8[sizeof(glm::vec3) * numVertices * 2 + sizeof(u32) * numIndices];
		glm::vec3* vertices = (glm::vec3*)storage;
		glm::vec3* normals = (glm::vec3*)(storage + sizeof(glm::vec3) * numVertices);
		u32* indices = (u32*)(storage + 2 * sizeof(glm::vec3) * numVertices);

		float dTheta = 2.0f * glm::pi<float>() / numSlices;
		float dPhi = glm::pi<float>() / numStacks;

		int vertexIndex = 0;
		int indexIndex = 0;

		// Generate vertices and normals
		for (int stack = 0; stack <= numStacks; stack++) {
			float phi = stack * dPhi;
			for (int slice = 0; slice <= numSlices; slice++) {
				float theta = slice * dTheta;

				float x = radius * sin(phi) * cos(theta);
				float y = radius * cos(phi);
				float z = radius * sin(phi) * sin(theta);

				vertices[vertexIndex] = glm::vec3(x, y, z);
				normals[vertexIndex] = glm::normalize(vertices[vertexIndex] - position);
				vertexIndex++;
			}
		}

		// Generate indices
		for (int stack = 0; stack < numStacks; stack++) {
			for (int slice = 0; slice < numSlices; slice++) {
				int v1 = stack * (numSlices + 1) + slice;
				int v2 = v1 + 1;
				int v3 = (stack + 1) * (numSlices + 1) + slice;
				int v4 = v3 + 1;

				indices[indexIndex++] = v1;
				indices[indexIndex++] = v2;
				indices[indexIndex++] = v3;

				indices[indexIndex++] = v2;
				indices[indexIndex++] = v4;
				indices[indexIndex++] = v3;
			}
		}

		return std::make_tuple(vertices, normals, indices, numVertices, numIndices);
	}
}
