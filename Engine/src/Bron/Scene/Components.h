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

#include <string>
#include <vector>

#include "Bron/Graphics/CameraView.h"
#include "Bron/Graphics/MaterialBase.h"
#include "Bron/Scene/Asset.h"
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

// For Component constraining
template<typename... Ts>
struct ComponentList {};

// Default component constraints: None
struct DefaultComponentTraits {
	using Requires = ComponentList<>;
	using Conflicts = ComponentList<>;
	using ParentRequiresAnyOf = ComponentList<>;
};

template<typename T>
struct ComponentTraits : DefaultComponentTraits {};

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
			o_position_ = position;
			o_rotation_quat_ = rotation_quat;
			o_scaling_ = scaling;

			glm::mat4 rotation = glm::toMat4(glm::quat(rotation_quat));

			matrix_ = glm::translate(glm::mat4(1.0f), position) * rotation * glm::scale(glm::mat4(1.0f), scaling);
		}
		return matrix_;
	}

	bool IsDirty() const {
		BR_PROFILE_FUNCTION();
		return !(CompareFloatsBits(position, o_position_) &&
				 CompareFloatsBits((glm::vec4*) (&rotation_quat), (glm::vec4*) (&o_rotation_quat_)) &&
				 CompareFloatsBits(scaling, o_scaling_));
	}

	TransformComponent() :
		position(0.0), rotation_quat({1.0f, 0.0f, 0.0f, 0.0f}), scaling(1.0), matrix_(1.0f), o_position_(0.0),
		o_rotation_quat_({1.0f, 0.0f, 0.0f, 0.0f}), o_scaling_(1.0) {}

	operator glm::mat4&() { return GetMatrix(); }
	glm::mat4& operator*() { return GetMatrix(); }

	glm::vec3 position;
	glm::quat rotation_quat; // w,x,y,z
	glm::vec3 scaling;

private:
	glm::mat4 matrix_;

	glm::vec3 o_position_;
	glm::quat o_rotation_quat_;
	glm::vec3 o_scaling_;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(TransformComponent, position, rotation_quat, scaling)
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
// What an entity draws and what it draws it with. Both are assets, shared by every entity
// that uses them; placing a model copies its choice of mesh and material in here, and from
// then on this is the only place the renderer looks.
struct MeshMaterialComponent {
	assets::AssetHandle mesh = assets::kNullHandle;
	assets::AssetHandle material = assets::kNullHandle;

	MeshMaterialComponent() = default;
	MeshMaterialComponent(const assets::AssetHandle& mesh, const assets::AssetHandle& material) :
		mesh(mesh), material(material) {}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(MeshMaterialComponent, mesh, material)
};


// --------------------------------------------------------------------
// Material workflow
// --------------------------------------------------------------------

NLOHMANN_JSON_SERIALIZE_ENUM(MaterialWorkflow, {
													   {kPhong, "phong"},
											   })


// --------------------------------------------------------------------
// Point light
// --------------------------------------------------------------------

struct PointLightComponent {
	glm::vec3 color{1.0f};

	PointLightComponent() = default;
	explicit PointLightComponent(const glm::vec3& c) : color(c) {}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(PointLightComponent, color)
};

template<>
struct ComponentTraits<PointLightComponent> : DefaultComponentTraits {
	using Requires = ComponentList<TransformComponent>;
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
	std::vector<assets::AssetHandle> scripts;

	ScriptComponent() = default;
	explicit ScriptComponent(const assets::AssetHandle& location) : scripts({location}) {}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ScriptComponent, scripts)
};

struct Box2D {
	glm::vec2 min, max;
};

// --------------------------------------------------------------------
// CanvasComponent
// --------------------------------------------------------------------

struct CanvasComponent {
	enum class Mode { kScreenSpace, kWorldSpace };

	/// World-space canvasses get draw before screen-space
	/// Use screen-space for UI and such, use world-space for rendering in the 3d world.
	Mode mode = Mode::kScreenSpace;

	/// The size the UI is designed at. In world space it is the root rect that
	/// RectTransformComponents resolve against. In screen space only its height counts:
	/// the canvas is scaled so that height fills the screen, and its width follows the
	/// screen's aspect. The root rect always starts at (0, 0).
	glm::vec2 reference_size = {1920.0f, 1080.0f};

	/// 100.0f canvas pixels = 1 world-space unit
	float pixels_per_unit = 100.0f;

	/// Draw order for screen-space, when there are multiple canvasses.
	/// Hierarchy of canvasses in the scene is irrelevant, sort_order is.
	i32 sort_order = 0;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(CanvasComponent, mode, reference_size, pixels_per_unit, sort_order)

	CanvasComponent() = default;
};

// --------------------------------------------------------------------
// RectTransformComponent
// --------------------------------------------------------------------

struct RectTransformComponent {
	// Where the rect's corners sit inside the parent's rect: (0, 0) its bottom-left, (1, 1)
	// its top-right. Equal anchors pin the rect to a point; apart, it stretches with the parent.
	glm::vec2 anchor_min{0.0f}, anchor_max{0.0f};

	// Pixels added to each anchor, giving the rect's corners.
	glm::vec2 offset_min{0.0f}, offset_max{100.0f};

	// The point scale and rotation happen around, in the rect itself: (0, 0) its
	// bottom-left, (1, 1) its top-right.
	glm::vec2 pivot{0.5f};

	glm::vec2 scale{1.0f};

	// Counter-clockwise, in degrees.
	float rotation = 0.0f;

	RectTransformComponent() = default;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(RectTransformComponent, anchor_min, anchor_max, offset_min, offset_max, pivot, scale,
								   rotation)

	// The rect in its parent's space, before this rect's own scale and rotation.
	[[nodiscard]] Box2D Rect(const Box2D& parent) const;

	// Scales and rotates 'rect' around the pivot. Applies to everything drawn in the rect,
	// children included.
	[[nodiscard]] glm::mat4 Local(const Box2D& rect) const;
};

template<>
struct ComponentTraits<RectTransformComponent> : DefaultComponentTraits {
	using Conflicts = ComponentList<TransformComponent>;
	using ParentRequiresAnyOf = ComponentList<RectTransformComponent, CanvasComponent>;
};

// --------------------------------------------------------------------
// Text2D
// --------------------------------------------------------------------

struct Text2DComponent {
	// Font MUST refer to a valid font in the asset manager
	assets::AssetHandle font;
	std::string text = "Sample text";
	float font_size = 30;
	glm::vec4 color{1.0};

	Text2DComponent() = default;
	Text2DComponent(const assets::AssetHandle& font) : font(font) {}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Text2DComponent, font, text, font_size, color)
};

template<>
struct ComponentTraits<Text2DComponent> : DefaultComponentTraits {
	using Requires = ComponentList<RectTransformComponent>;
};

// --------------------------------------------------------------------
// Rectangle
// --------------------------------------------------------------------
// Fills its rect with a colour.
struct Box2DComponent {
	glm::vec4 color{1.0f};

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Box2DComponent, color);
};

template<>
struct ComponentTraits<Box2DComponent> : DefaultComponentTraits {
	using Requires = ComponentList<RectTransformComponent>;
};

// --------------------------------------------------------------------
// All Components
// --------------------------------------------------------------------

using AllComponents =
		ComponentList<IDComponent, TagComponent, TransformComponent, HierarchyComponent, MeshMaterialComponent,
					  PointLightComponent, VisibilityComponent, CameraComponent, ScriptComponent, CanvasComponent,
					  RectTransformComponent, Text2DComponent, Box2DComponent>;

} // namespace bron
