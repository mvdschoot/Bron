
#include "Mesh.h"

namespace Steve
{
	glm::vec3 Mesh::FindCentroid(const glm::vec3 *vertices, const uint64_t n) {
		glm::vec3 res(0.0);

		for (usize i = 0; i < n; i++) {
			res += vertices[i];
		}

		return res / static_cast<float>(n);
	}

	void Mesh::GenerateVao(const NamedBufferLayout<VertexVariables>& bufferLayout) {
		// MultipleNamedBufferData will place the data at the correct places.
		MultipleNamedBufferData<VertexVariables> vertexBuffers(&bufferLayout, vertexData.positions.size());

		// Set the data. The MultipleNamedBufferData objects will take care of placement of the data.
		for (const VertexVariables variable : bufferLayout.Data) {
			switch (variable)
			{
				case POSITIONS:
					vertexBuffers.Set(VertexVariables::POSITIONS, reinterpret_cast<u8*>(&vertexData.positions[0]));
					break;

				case NORMALS:
					CORE_ASSERT(vertexData.normals.has_value(),
						"Shader requires the normal coordinates of the vertices, but they have not been given");
					vertexBuffers.Set(VertexVariables::NORMALS, reinterpret_cast<u8*>(&vertexData.normals.value()[0]));
					break;

				case UVS:
					CORE_ASSERT(vertexData.uvs.has_value(),
						"Shader requires the UV coordinates of the vertices, but they have not been given");
					vertexBuffers.Set(VertexVariables::UVS, reinterpret_cast<u8*>(&vertexData.uvs.value()[0]));
					break;

				case TANGENTS:
					CORE_ASSERT(vertexData.tangents.has_value(),
						"Shader requires the tangents of the vertices, but they have not been given");
					vertexBuffers.Set(VertexVariables::TANGENTS, reinterpret_cast<u8*>(&vertexData.tangents.value()[0]));
					break;

				default:
					CORE_ASSERT(false, "Unknown vertex variable type provided to vertexBuffers.Set()");
					break;
			}

		}


		vao = VertexArray::Create();

		// Add the vertex buffer to the vao
		const Ref<VertexBuffer> b = VertexBuffer::Create(reinterpret_cast<float*>(vertexBuffers.Data), vertexBuffers.GetTotalSize());
		b->setBufferLayout(bufferLayout);
		vao->addVertexBuffer(b);

		// Add the index buffer to the vao
		const Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(&vertexData.indices[0], vertexData.indices.size());
		vao->setIndexBuffer(indexBuffer);
	}

	Ref<VertexArray> Mesh::GetVao(const NamedBufferLayout<VertexVariables>& bufferLayout) {
		if (vao == nullptr) {
			GenerateVao(bufferLayout);
		}

		return vao;
	}
} // namespace Steve
