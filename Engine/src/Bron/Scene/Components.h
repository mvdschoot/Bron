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
#include "Bron/Graphics/CameraView.h"
#include "Bron/Graphics/MaterialBase.h"
#include "Bron/Graphics/VertexArray.h"
#include "Serialization/GlmJson.h"
#include "nlohmann/json.hpp"

namespace bron {
// nlohmann has no idea what a UUID is; it round trips as its string form.
inline void to_json(nlohmann::json& j, const UUID& uuid) { j = uuid.value; }

inline void from_json(const nlohmann::json& j, UUID& path) {
	const std::string text = j.get<std::string>();
	std::strncpy(path.value, text.c_str(), sizeof(path.value) - 1);
	path.value[sizeof(path.value) - 1] = '\0';
}

inline void to_json(nlohmann::json& j, const std::filesystem::path& path) { j = path.generic_string(); }

inline void from_json(const nlohmann::json& j, std::filesystem::path& path) {
	const std::string text = j.get<std::string>();
	path = std::filesystem::path(text);
}

// A save file cannot key entities by entt::entity: those are positions in a
// registry, so they only mean anything in the registry that produced them.
// Every entity carries a stable identifier instead.
struct IDComponent {
	UUID id;

	IDComponent() = default;
	explicit IDComponent(const UUID& uuid) : id(uuid) {}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(IDComponent, id)
};

// --------------------------------------------------------------------
// Tag
// --------------------------------------------------------------------

struct TagComponent {
	std::string name = "Default name";

	TagComponent() = default;
	explicit TagComponent(std::string n) : name(std::move(n)) {}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(TagComponent, name)
};


// --------------------------------------------------------------------
// Transform
// --------------------------------------------------------------------

struct TransformComponent {
	glm::mat4& GetMatrix() {
		if (IsDirty()) {
			OPosition = Position;
			ORotationQuat = RotationQuat;
			OScaling = Scaling;

			glm::mat4 rotation = glm::toMat4(glm::quat(RotationQuat));

			Matrix = glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scaling);
		}
		return Matrix;
	}

	bool IsDirty() const {
		BR_PROFILE_FUNCTION();
		return !(CompareFloatsBits(Position, OPosition) &&
				 CompareFloatsBits((glm::vec4*) (&RotationQuat), (glm::vec4*) (&ORotationQuat)) &&
				 CompareFloatsBits(Scaling, OScaling));
	}

	TransformComponent() :
		Position(0.0), RotationQuat({1.0f, 0.0f, 0.0f, 0.0f}), Scaling(1.0), Matrix(1.0f), OPosition(0.0),
		ORotationQuat({1.0f, 0.0f, 0.0f, 0.0f}), OScaling(1.0) {}

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

	template<typename BasicJsonType,
			 nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0>
	friend void to_json(BasicJsonType& nlohmann_json_j, const TransformComponent& nlohmann_json_t) {
		NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, Position, RotationQuat, Scaling))
	}
	template<typename BasicJsonType,
			 nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0>
	friend void from_json(const BasicJsonType& nlohmann_json_j, TransformComponent& nlohmann_json_t) {
		NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, Position, RotationQuat, Scaling))
	}
};


// --------------------------------------------------------------------
// Hierarchy
// --------------------------------------------------------------------

struct HierarchyComponent {
	entt::entity parent = entt::null;
	std::vector<entt::entity> children;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(HierarchyComponent, parent, children)
};


// --------------------------------------------------------------------
// Mesh
// --------------------------------------------------------------------

enum VertexVariables { kPositions, kNormals, kUvs, kTangents, kSmoothNormals };

struct MeshData {
	std::vector<glm::vec3> positions;
	std::vector<u32> indices;
	std::optional<std::vector<glm::vec3>> normals;
	std::optional<std::vector<glm::vec2>> uvs;
	std::optional<std::vector<glm::vec3>> tangents;

	// Normals with the hard edges welded shut - see SmoothNormals(). Derived from the
	// positions and indices, so it is filled in on demand rather than by the importer.
	std::optional<std::vector<glm::vec3>> smooth_normals;
};

// TODO: store buffer layout as well, or better define packages of materials, shaders, buffer layouts, etc.
struct MeshComponent {
	MeshData vertex_data;
	Ref<MaterialBase> material;

	// Lazily built by GetVao(); not part of the mesh's data.
	Ref<VertexArray> vao = nullptr;
};

// Average of the given vertices.
glm::vec3 FindCentroid(const glm::vec3* vertices, uint64_t n);

// Returns the mesh's vertex array, building it against the given layout on first use.
Ref<VertexArray> GetVao(MeshComponent& mesh, const NamedBufferLayout<VertexVariables>& buffer_layout);


// --------------------------------------------------------------------
// Model source
// --------------------------------------------------------------------

NLOHMANN_JSON_SERIALIZE_ENUM(MaterialWorkflow, {
													   {kPhong, "phong"},
											   })

// Marks an entity as the root of an imported model. Meshes are never written
// to a save file - they are re-imported from this path on load, and
// everything below this entity is treated as generated output.
struct ModelSourceComponent {
	std::string path; // relative to paths::AssetRoot()
	MaterialWorkflow workflow = kPhong;

	ModelSourceComponent() = default;
	ModelSourceComponent(std::string p, const MaterialWorkflow w) : path(std::move(p)), workflow(w) {}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ModelSourceComponent, path, workflow)
};


// --------------------------------------------------------------------
// Point light
// --------------------------------------------------------------------

struct PointLightComponent {
	glm::vec3 color{1.0f};

	PointLightComponent() = default;
	explicit PointLightComponent(const glm::vec3& c) : color(c) {}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(PointLightComponent, color)
};

// --------------------------------------------------------------------
// Visibility
// --------------------------------------------------------------------

struct VisibilityComponent {
	bool visible = true;

	VisibilityComponent() = default;
	explicit VisibilityComponent(const bool visible) : visible(visible) {}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(VisibilityComponent, visible)
};

// --------------------------------------------------------------------
// Camera
// --------------------------------------------------------------------

enum ProjectionType { kPerspective, kOrthographic };

NLOHMANN_JSON_SERIALIZE_ENUM(ProjectionType, {
													 {kPerspective, "perspective"},
													 {kOrthographic, "orthographic"},
											 })

// A point of view that belongs to the scene rather than to the editor.
//
// Data only: where the camera is and which way it faces is its entity's
// TransformComponent, and the matrices a shader wants are a CameraView, derived from
// the two whenever something draws. Nothing here is a Camera object, so there is no
// second copy of a pose to keep in step.
//
// The aspect ratio is deliberately absent. It belongs to whatever is being drawn into -
// the editor's viewport panel, the game's window - and the same scene has a different
// one in each, so storing it in the file would be storing one caller's accident.
struct CameraComponent {
	ProjectionType projection = kPerspective;

	// Vertical field of view, radians. Perspective only, but kept across a switch to
	// orthographic so toggling back does not lose it.
	float fov_y = glm::radians(45.0f);

	// Vertical extent in world units; the horizontal one follows from the aspect of the
	// render target. Orthographic only.
	float ortho_size = 10.0f;

	// Not 'near' and 'far': both are macros in windef.h, and the errors that causes are
	// a long way from the cause.
	float near_plane = 0.1f;
	float far_plane = 1000.0f;

	// The camera the runtime looks through. A scene has exactly one - see
	// Scene::PrimaryCamera(), and the editor clears the flag on the others when one is
	// set.
	bool primary = false;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(CameraComponent, projection, fov_y, ortho_size, near_plane, far_plane, primary)
};

// The view matrix and projection for looking through 'camera', whose entity sits at
// 'world_transform'. 'aspect' is the render target's width over its height.
CameraView ViewFrom(const CameraComponent& camera, const glm::mat4& world_transform, float aspect);

// --------------------------------------------------------------------
// Script
// --------------------------------------------------------------------
struct ScriptComponent {
	std::vector<std::filesystem::path> scripts;

	ScriptComponent() = default;
	explicit ScriptComponent(const std::filesystem::path& location) : scripts({location}) {}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ScriptComponent, scripts)
};

} // namespace bron
