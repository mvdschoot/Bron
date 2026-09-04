#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/UUID.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Util/Util.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <entt/entity/registry.hpp>

#include <optional>
#include <string>
#include <vector>

#include "Bron/Graphics/Components/BufferExtentions.h"
#include "Bron/Graphics/MaterialBase.h"
#include "Bron/Graphics/VertexArray.h"
#include "Serialization/GlmJson.h"
#include "nlohmann/json.hpp"

namespace bron
{
	// nlohmann has no idea what a UUID is; it round trips as its string form.
	inline void to_json(nlohmann::json& j, const UUID& uuid)
	{
		j = uuid.value;
	}

	inline void from_json(const nlohmann::json& j, UUID& uuid)
	{
		const std::string text = j.get<std::string>();
		std::strncpy(uuid.value, text.c_str(), sizeof(uuid.value) - 1);
		uuid.value[sizeof(uuid.value) - 1] = '\0';
	}

	// A save file cannot key entities by entt::entity: those are positions in a
	// registry, so they only mean anything in the registry that produced them.
	// Every entity carries a stable identifier instead.
	struct IDComponent
	{
		UUID id;

		IDComponent() = default;
		explicit IDComponent(const UUID& uuid) : id(uuid) {}

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(IDComponent, id)
	};

	// --------------------------------------------------------------------
	// Tag
	// --------------------------------------------------------------------

	struct TagComponent
	{
		std::string name = "Default name";

		TagComponent() = default;
		explicit TagComponent(std::string n) : name(std::move(n)) {}

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(TagComponent, name)
	};


	// --------------------------------------------------------------------
	// Transform
	// --------------------------------------------------------------------

	struct TransformComponent
	{
		glm::mat4& GetMatrix()
		{
			if (IsDirty())
			{
				OPosition = Position;
				ORotationQuat = RotationQuat;
				OScaling = Scaling;

				glm::mat4 rotation = glm::toMat4(glm::quat(RotationQuat));

				Matrix = glm::translate(glm::mat4(1.0f), Position)
					* rotation
					* glm::scale(glm::mat4(1.0f), Scaling);
			}
			return Matrix;
		}

		bool IsDirty() const
		{
			BR_PROFILE_FUNCTION();
			return !(CompareFloatsBits(Position, OPosition)
				&& CompareFloatsBits((glm::vec4*)(&RotationQuat), (glm::vec4*)(&ORotationQuat))
				&& CompareFloatsBits(Scaling, OScaling));
		}

		TransformComponent() : Position(0.0),
			RotationQuat({1.0f, 0.0f, 0.0f, 0.0f}), Scaling(1.0), Matrix(1.0f),
			OPosition(0.0), ORotationQuat({1.0f, 0.0f, 0.0f, 0.0f}), OScaling(1.0) {}

		operator glm::mat4&() { return GetMatrix(); }
		glm::mat4& operator*() { return GetMatrix(); }

		glm::vec3 Position;
		glm::quat RotationQuat; // w,x,y,z
		glm::vec3 Scaling;

	private:
		glm::mat4 Matrix;

		glm::vec3 OPosition;
		glm::quat ORotationQuat;
		glm::vec3 OScaling;

		template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> friend void to_json(BasicJsonType& nlohmann_json_j, const TransformComponent& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, Position, RotationQuat, Scaling)) } template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> friend void from_json(const BasicJsonType& nlohmann_json_j, TransformComponent& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, Position, RotationQuat, Scaling)) }
	};


	// --------------------------------------------------------------------
	// Hierarchy
	// --------------------------------------------------------------------

	struct HierarchyComponent
	{
		entt::entity parent = entt::null;
		std::vector<entt::entity> children;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(HierarchyComponent, parent, children)
	};


	// --------------------------------------------------------------------
	// Mesh
	// --------------------------------------------------------------------

	enum VertexVariables
	{
		POSITIONS,
		NORMALS,
		UVS,
		TANGENTS
	};

	struct MeshData
	{
		std::vector<glm::vec3> positions;
		std::vector<u32> indices;
		std::optional<std::vector<glm::vec3>> normals;
		std::optional<std::vector<glm::vec2>> uvs;
		std::optional<std::vector<glm::vec3>> tangents;
	};

	struct MeshComponent
	{
		MeshData vertexData;
		Ref<MaterialBase> material;

		// Lazily built by GetVao(); not part of the mesh's data.
		Ref<VertexArray> vao = nullptr;
	};

	// Average of the given vertices.
	glm::vec3 FindCentroid(const glm::vec3* vertices, uint64_t n);

	// Returns the mesh's vertex array, building it against the given layout on first use.
	Ref<VertexArray> GetVao(MeshComponent& mesh, const NamedBufferLayout<VertexVariables>& bufferLayout);


	// --------------------------------------------------------------------
	// Model source
	// --------------------------------------------------------------------

	NLOHMANN_JSON_SERIALIZE_ENUM(MaterialWorkflow, {
		{ PHONG, "phong" },
	})

	// Marks an entity as the root of an imported model. Meshes are never written
	// to a save file - they are re-imported from this path on load, and
	// everything below this entity is treated as generated output.
	struct ModelSourceComponent
	{
		std::string path; // relative to paths::AssetRoot()
		MaterialWorkflow workflow = PHONG;

		ModelSourceComponent() = default;
		ModelSourceComponent(std::string p, const MaterialWorkflow w) : path(std::move(p)), workflow(w) {}

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(ModelSourceComponent, path, workflow)
	};


	// --------------------------------------------------------------------
	// Point light
	// --------------------------------------------------------------------

	struct PointLightComponent
	{
		glm::vec3 color{1.0f};

		PointLightComponent() = default;
		explicit PointLightComponent(const glm::vec3& c) : color(c) {}

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(PointLightComponent, color)
	};
}
