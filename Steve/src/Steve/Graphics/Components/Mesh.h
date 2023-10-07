#ifndef __MESHCOMPONENT_HEADR__
#define __MESHCOMPONENT_HEADR__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "Steve/Graphics/Texture.h"
#include "Steve/Graphics/Buffer.h"
#include "Steve/Graphics/VertexArray.h"
#include "Steve/Graphics/Shader.h"

#include <map>

#include "MiscellaneousComponents.h"
#include "Steve/ECS/Entity.h"
#include "Steve/Graphics/Camera.h"
#include "Steve/Graphics/Renderer/2D.h"

#include "Context.h"
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Scene/Node.h"

namespace Steve
{
	class Mesh : public Node
	{
	public:
		Mesh(RegistryData* reg, const BufferLayout* layout)
			: Node(reg), mBufferLayout(layout)
		{
			type |= NodeType_Mesh;
		}

		Mesh(RegistryData* reg,
			const BufferLayout* layout,
			void* vertex_data, usize vertex_size,
			u32* index_data, usize index_count,
			MeshContext* context) :
				Node(reg),
				pContext(context),
				pVao(VertexArray::Create()),
				mBufferLayout(layout)
		{
			setVertexData(vertex_data, vertex_size);
			setIndexData(index_data, index_count);
			type |= NodeType_Mesh;
		}

		Mesh& operator=(Mesh&& comp) noexcept
		{
			return *this;
		}

		void setVertexData(void* vertex_data, usize vertex_data_size);
		void setIndexData(u32* index_data, u32 index_count);

		glm::vec3 static FindCentroid(glm::vec3* vertices, uint64_t n);

		MeshContext* pContext;
		Ref<VertexArray> pVao;
	private:
		const BufferLayout* mBufferLayout;
	};
}

#endif