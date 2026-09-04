#include "Components.h"

#include "Bron/Graphics/Buffer.h"
#include "Bron/Graphics/Components/BufferExtentions.h"

namespace bron {
glm::vec3 FindCentroid(const glm::vec3* vertices, const uint64_t n) {
	glm::vec3 res(0.0);

	for (usize i = 0; i < n; i++) {
		res += vertices[i];
	}

	return res / static_cast<float>(n);
}

static void GenerateVao(MeshComponent& mesh, const NamedBufferLayout<VertexVariables>& buffer_layout) {
	MeshData& vertex_data = mesh.vertex_data;

	// MultipleNamedBufferData will place the data at the correct places.
	MultipleNamedBufferData<VertexVariables> vertex_buffers(&buffer_layout, vertex_data.positions.size());

	// Set the data. The MultipleNamedBufferData objects will take care of placement of the data.
	for (const VertexVariables variable: buffer_layout.keys) {
		switch (variable) {
			case kPositions:
				vertex_buffers.Set(VertexVariables::kPositions, reinterpret_cast<u8*>(&vertex_data.positions[0]));
				break;

			case kNormals:
				BR_CORE_ASSERT(vertex_data.normals.has_value(),
							   "Shader requires the normal coordinates of the vertices, but they have not been given");
				vertex_buffers.Set(VertexVariables::kNormals, reinterpret_cast<u8*>(&vertex_data.normals.value()[0]));
				break;

			case kUvs:
				BR_CORE_ASSERT(vertex_data.uvs.has_value(),
							   "Shader requires the UV coordinates of the vertices, but they have not been given");
				vertex_buffers.Set(VertexVariables::kUvs, reinterpret_cast<u8*>(&vertex_data.uvs.value()[0]));
				break;

			case kTangents:
				BR_CORE_ASSERT(vertex_data.tangents.has_value(),
							   "Shader requires the tangents of the vertices, but they have not been given");
				vertex_buffers.Set(VertexVariables::kTangents, reinterpret_cast<u8*>(&vertex_data.tangents.value()[0]));
				break;

			default:
				BR_CORE_ASSERT(false, "Unknown vertex variable type provided to vertexBuffers.Set()");
				break;
		}
	}

	mesh.vao = VertexArray::Create();

	// Add the vertex buffer to the vao
	const Ref<VertexBuffer> b =
			VertexBuffer::Create(reinterpret_cast<float*>(vertex_buffers.data), vertex_buffers.GetTotalSize());
	b->SetBufferLayout(buffer_layout);
	mesh.vao->AddVertexBuffer(b);

	// Add the index buffer to the vao
	const Ref<IndexBuffer> index_buffer = IndexBuffer::Create(&vertex_data.indices[0], vertex_data.indices.size());
	mesh.vao->SetIndexBuffer(index_buffer);
}

Ref<VertexArray> GetVao(MeshComponent& mesh, const NamedBufferLayout<VertexVariables>& buffer_layout) {
	if (mesh.vao == nullptr) {
		GenerateVao(mesh, buffer_layout);
	}

	return mesh.vao;
}
} // namespace bron
