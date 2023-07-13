#include "RenderComponents.h"
#include "Standard.h"

namespace Steve::graphics
{
	void Mesh::setVertexData(void* vertex_data, uint64_t vertex_data_size)
	{
		const Ref<VertexBuffer> b = VertexBuffer::Create((float*)vertex_data, vertex_data_size);
		b->setBufferLayout(mBufferLayout);
		pVao->addVertexBuffer(b);
	}

	void Mesh::setIndexData(uint32_t* index_data, uint32_t index_count)
	{
		const Ref<IndexBuffer> b = IndexBuffer::Create(index_data, index_count);
		pVao->setIndexBuffer(b);
	}

	void PointLight::ChangePosition(glm::vec3 new_position)
	{
		GetComponent<LightData>()->Set(LightDataTypes::Position, new_position);

		glm::mat4& trans = **GetComponent<TransformComponent>();
		trans[3][0] = new_position[0];
		trans[3][1] = new_position[1];
		trans[3][2] = new_position[2];
	}

	void PointLight::ChangeColor(glm::vec3 new_color)
	{
		GetComponent<LightData>()->Set(LightDataTypes::Color, new_color);
		Meshes[0].pContext->pMaterial->Set(MaterialDataTypes::Diffuse, new_color);
		Meshes[0].pContext->pMaterial->Set(MaterialDataTypes::Specular,new_color);
	}

	glm::vec3 PointLight::GetPosition()
	{
		return *(glm::vec3*)GetComponent<LightData>()->Get(LightDataTypes::Position);
	}

	glm::vec3 PointLight::GetColor()
	{
		return *(glm::vec3*)GetComponent<LightData>()->Get(LightDataTypes::Color);
	}

	std::tuple<glm::vec3*, glm::vec3*, u32*> GenCubeSmoothVertices(glm::vec3 position, glm::vec3 dimensions)
	{
		usize sizeVectors = 24 * sizeof(glm::vec3);
		usize sizeIndices = 36 * sizeof(u32);
		u8* storage = new u8[(2 * sizeVectors) + sizeIndices];

		glm::vec3* positions = (glm::vec3*)storage;
		glm::vec3* normals = (glm::vec3*)(storage + sizeVectors);
		u32* indices = (u32*)(storage + (2 * sizeVectors));

		const float x = position.x;
		const float y = position.y;
		const float z = position.z;
		const float w = dimensions.x;
		const float h = dimensions.y;
		const float d = dimensions.z;

		positions[0] = glm::vec3(x, y, z);
		positions[1] = glm::vec3(x + w, y, z);
		positions[2] = glm::vec3(x + w, y + h, z);
		positions[3] = glm::vec3(x, y + h, z);
		positions[4] = glm::vec3(x + w, y, z + d);
		positions[5] = glm::vec3(x, y, z + d);
		positions[6] = glm::vec3(x, y + h, z + d);
		positions[7] = glm::vec3(x + w, y + h, z + d);
		positions[8] = glm::vec3(x, y + h, z);
		positions[9] = glm::vec3(x + w, y + h, z);
		positions[10] = glm::vec3(x + w, y + h, z + d);
		positions[11] = glm::vec3(x, y + h, z + d);
		positions[12] = glm::vec3(x, y, z);
		positions[13] = glm::vec3(x, y + h, z);
		positions[14] = glm::vec3(x, y + h, z + d);
		positions[15] = glm::vec3(x, y, z + d);
		positions[16] = glm::vec3(x + w, y, z);
		positions[17] = glm::vec3(x + w, y, z + d);
		positions[18] = glm::vec3(x + w, y + h, z + d);
		positions[19] = glm::vec3(x + w, y + h, z);
		positions[20] = glm::vec3(x, y, z + d);
		positions[21] = glm::vec3(x + w, y, z + d);
		positions[22] = glm::vec3(x + w, y, z);
		positions[23] = glm::vec3(x, y, z);

		// Set indices
		indices[0] = 0; indices[1] = 1; indices[2] = 2;
		indices[3] = 2; indices[4] = 3; indices[5] = 0;
		indices[6] = 4; indices[7] = 5; indices[8] = 6;
		indices[9] = 6; indices[10] = 7; indices[11] = 4;
		indices[12] = 8; indices[13] = 9; indices[14] = 10;
		indices[15] = 10; indices[16] = 11; indices[17] = 8;
		indices[18] = 12; indices[19] = 13; indices[20] = 14;
		indices[21] = 14; indices[22] = 15; indices[23] = 12;
		indices[24] = 16; indices[25] = 17; indices[26] = 18;
		indices[27] = 18; indices[28] = 19; indices[29] = 16;
		indices[30] = 20; indices[31] = 21; indices[32] = 22;
		indices[33] = 22; indices[34] = 23; indices[35] = 20;

		// Initialize all vertex normals to zero
		for (int i = 0; i < 24; i++) {
			normals[i] = glm::vec3(0.0f);
		}

		// Compute the normal of each face and add it to the vertex normals of its three vertices
		for (int i = 0; i < 36; i += 3) {
			glm::vec3 v1 = positions[indices[i]];
			glm::vec3 v2 = positions[indices[i + 1]];
			glm::vec3 v3 = positions[indices[i + 2]];
			const glm::vec3 faceNormal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
			normals[indices[i]] += faceNormal;
			normals[indices[i + 1]] += faceNormal;
			normals[indices[i + 2]] += faceNormal;
		}

		// Normalize all vertex normals
		for (int i = 0; i < 24; i++) {
			normals[i] = glm::normalize(normals[i]);
		}

		return std::make_tuple(positions, normals, indices);
	}
}