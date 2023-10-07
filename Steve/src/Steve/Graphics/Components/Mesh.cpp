
#include "Mesh.h"

namespace Steve
{
	void Mesh::setVertexData(void* vertex_data, uint64_t vertex_data_size)
	{
		const Ref<VertexBuffer> b = VertexBuffer::Create((float*)vertex_data, vertex_data_size);
		b->setBufferLayout(*mBufferLayout);
		pVao->addVertexBuffer(b);
	}

	void Mesh::setIndexData(uint32_t* index_data, uint32_t index_count)
	{
		const Ref<IndexBuffer> b = IndexBuffer::Create(index_data, index_count);
		pVao->setIndexBuffer(b);
	}

	glm::vec3 Mesh::FindCentroid(glm::vec3* vertices, uint64_t n)
	{
		glm::vec3 res(0.0);

		for(usize i = 0; i < n; i++)
		{
			res += vertices[i];
		}

		return res / (float)n;
	}
}
