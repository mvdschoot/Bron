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
#include "Steve/Graphics/MaterialBase.h"
#include "Steve/Graphics/Renderer/2D.h"

#include "Steve/Scene/Node.h"

namespace Steve {
	enum VertexVariables {
		POSITIONS,
		NORMALS,
		UVS,
		TANGENTS
	};

	struct MeshData {
		std::vector<glm::vec3> positions;
		std::vector<u32> indices;
		std::optional<std::vector<glm::vec3>> normals;
		std::optional<std::vector<glm::vec2>> uvs;
		std::optional<std::vector<glm::vec3>> tangents;
	};

	class Mesh : public Node
	{
	public:
		explicit Mesh(RegistryData* reg, MeshData&& meshData, const Ref<MaterialBase> material)
				: Node(reg), vertexData(meshData), material(material) {
			type |= NodeType_Mesh;
		}

		Mesh& operator=(Mesh&& comp) noexcept
		{
			return *this;
		}

		static glm::vec3 FindCentroid(const glm::vec3* vertices, uint64_t n);
		Ref<VertexArray> GetVao(const NamedBufferLayout<VertexVariables>& bufferLayout);
		Ref<MaterialBase> getMaterial() const { return material;}

	private:
		void GenerateVao(const NamedBufferLayout<VertexVariables>& bufferLayout);

		Ref<VertexArray> vao = nullptr;
		MeshData vertexData;
		Ref<MaterialBase> material;
	};
}

#endif