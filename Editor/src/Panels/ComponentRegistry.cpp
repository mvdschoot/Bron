#include "ComponentRegistry.h"

#include "imgui.h"
#include "nfd.hpp"
#include "Bron/Scene/AssetManager.h"
#include "Bron/Scripting/LuaManager.h"
#include "Bron/Util/Paths.h"
#include "Core/Icons.h"

namespace bron::editor {
using namespace ImGui;

namespace {
// ----------------------------------------------------------------
// Draw functions - the only part written per component type
// ----------------------------------------------------------------

void DrawTag(EditorContext& context, Scene& scene, const entt::entity entity) {
	TagComponent& tag = scene.reg.get<TagComponent>(entity);

	char buffer[256];
	std::strncpy(buffer, tag.name.c_str(), sizeof(buffer) - 1);
	buffer[sizeof(buffer) - 1] = '\0';

	if (InputText("Name", buffer, sizeof(buffer)))
		tag.name = buffer;
}

// Converting a quaternion to euler angles is lossy, so the result is cached per entity:
// recomputing it every frame makes a rotation drag jump around near the poles.
entt::entity s_euler_cache_owner = entt::null;
glm::vec3 s_euler_cache{0.0f};

void DrawTransform(EditorContext& context, Scene& scene, const entt::entity entity) {
	TransformComponent& t = scene.reg.get<TransformComponent>(entity);

	DragFloat3("Position", value_ptr(t.position));

	if (s_euler_cache_owner != entity) {
		s_euler_cache_owner = entity;
		s_euler_cache = glm::degrees(glm::eulerAngles(t.rotation_quat));
	}

	if (DragFloat3("Rotation", value_ptr(s_euler_cache)))
		t.rotation_quat = glm::quat(glm::radians(s_euler_cache));

	// Camera's may not be scaled
	if (!scene.reg.all_of<CameraComponent>(entity)) {
		DragFloat3("Scaling", value_ptr(t.scaling));
	}
}

void DrawHierarchy(EditorContext& context, Scene& scene, const entt::entity entity) {
	const HierarchyComponent& h = scene.reg.get<HierarchyComponent>(entity);

	if (h.parent == entt::null)
		TextDisabled("Parent: none");
	else
		Text("Parent: %s", scene.reg.get<TagComponent>(h.parent).name.c_str());

	Text("Children: %d", static_cast<int>(h.children.size()));
}

void DrawMesh(EditorContext& context, Scene& scene, const entt::entity entity) {
	const MeshMaterialComponent& component = scene.reg.get<MeshMaterialComponent>(entity);
	assets::AssetManager& manager = assets::AssetManager::Instance();

	if (const Ref<assets::MeshAsset> mesh = manager.Get<assets::MeshAsset>(component.mesh)) {
		Text("Vertices: %d", static_cast<int>(mesh->mesh_data.positions.size()));
		Text("Indices: %d", static_cast<int>(mesh->mesh_data.indices.size()));
	} else {
		TextDisabled("Mesh %s cannot be found", component.mesh.value);
	}

	if (const Ref<assets::MaterialAsset> material = manager.Get<assets::MaterialAsset>(component.material))
		Text("Shader: %s", material->material->shader_name);
	else
		TextDisabled("Material %s cannot be found; drawn with the default", component.material.value);

	const std::optional<std::filesystem::path> source = manager.SourceFile(component.mesh);
	TextDisabled("From: %s", source.has_value() ? source->generic_string().c_str() : "built in");
}

void DrawPointLight(EditorContext& context, Scene& scene, const entt::entity entity) {
	ColorEdit3("Color", value_ptr(scene.reg.get<PointLightComponent>(entity).color));
}

void DrawVisibility(EditorContext& context, Scene& scene, const entt::entity entity) {
	Checkbox("Visible", &scene.reg.get<VisibilityComponent>(entity).visible);
}

void DrawCamera(EditorContext& context, Scene& scene, const entt::entity entity) {
	CameraComponent& camera = scene.reg.get<CameraComponent>(entity);

	int projection_type = camera.projection == kPerspective ? 0 : 1;
	if (Combo("Projection", &projection_type, "Perspective\0Orthographic\0"))
		camera.projection = projection_type == 0 ? kPerspective : kOrthographic;

	// Primary is exclusive across the scene, so it is applied by clearing the flag everywhere
	// else rather than written straight into the component.
	if (bool is_primary = camera.primary; Checkbox("Primary camera", &is_primary) && is_primary) {
		for (auto [other_entity, other_camera]: scene.reg.view<CameraComponent>().each())
			other_camera.primary = false;
		camera.primary = true;
	}

	// Preview is editor state, not scene state: it lives in the context so the viewport can
	// read it, and it is deliberately not serialized with the camera.
	if (bool previewing = context.active_camera == entity; Checkbox("Preview", &previewing))
		context.active_camera = previewing ? entity : entt::null;

	InputFloat("FOV", &camera.fov_y);
	InputFloat("Near plane", &camera.near_plane);
	InputFloat("Far plane", &camera.far_plane);

	if (camera.projection == kOrthographic)
		InputFloat("Orthographic size", &camera.ortho_size);
}


void DrawScript(EditorContext& context, Scene& scene, const entt::entity entity) {
	ScriptComponent& script_component = scene.reg.get<ScriptComponent>(entity);

	if (BeginListBox("Attached scripts")) {
		for (auto& script: script_component.scripts) {
			std::string name = context.asset_manager.Metadata(script)->path.stem().string();
			Selectable(name.c_str());
		}

		EndListBox();
	}

	if (icons::Button(icons::Id::kFile, "Upload a script into the project")) {
		BR_CORE_ASSERT(NFD::Init(), "Failed to initialize the file picker");

		NFD::UniquePath out_path;
		nfdresult_t result = NFD::OpenDialog(out_path);
		NFD_Quit();

		if (result != NFD_OKAY) {
			BR_APP_INFO("User did not pick a folder for project export.");
			return;
		}

		std::filesystem::path script_file = out_path.get();

		if (script_file.extension() != ".lua" || !paths::InAssetDirectory(script_file)) {
			BR_APP_ERROR("You must pick a lua file inside the Asset directory");
			return;
		}

		// We don't check if LoadScript returns an empty optional, because we already guarantee that the file exists.
		assets::AssetHandle script_handle = context.asset_manager.LoadScript(script_file, kLua).value();
		scene.lua_manager->AttachScript(script_handle, entity);
		script_component.scripts.push_back(script_handle);
		BR_APP_INFO("Added script {} to entity {}", script_file.generic_string(), static_cast<u64>(entity));
	}
}

void DrawCanvas(EditorContext& context, Scene& scene, const entt::entity entity) {
	CanvasComponent& canvas = scene.reg.get<CanvasComponent>(entity);

	int mode = canvas.mode == CanvasComponent::Mode::kScreenSpace;

	Text("Mode");
	SameLine();
	RadioButton("Screen space", &mode, 1);
	SameLine();
	RadioButton("World space", &mode, 0);

	canvas.mode = mode == 0 ? CanvasComponent::Mode::kWorldSpace : CanvasComponent::Mode::kScreenSpace;

	if (canvas.mode == CanvasComponent::Mode::kWorldSpace) {
		InputFloat("Pixel per world unit", &canvas.pixels_per_unit);
	} else if (canvas.mode == CanvasComponent::Mode::kScreenSpace) {
		InputFloat2("Reference size", value_ptr(canvas.reference_size));
		InputInt("Draw layer", &canvas.sort_order);
	}
}

void DrawRectTransform(EditorContext& context, Scene& scene, const entt::entity entity) {
	RectTransformComponent& rect = scene.reg.get<RectTransformComponent>(entity);

	InputFloat2("Anchor min", value_ptr(rect.anchor_min));
	InputFloat2("Anchor max", value_ptr(rect.anchor_max));

	InputFloat2("Offset min", value_ptr(rect.offset_min));
	InputFloat2("Offset max", value_ptr(rect.offset_max));

	InputFloat2("Pivot", value_ptr(rect.pivot));

	InputFloat2("Scale", value_ptr(rect.scale));
	InputFloat("Rotation", &rect.rotation);
}

void DrawText2d(EditorContext& context, Scene& scene, const entt::entity entity) {
	Text2DComponent& text = scene.reg.get<Text2DComponent>(entity);

	char buf[4096] = {};
	text.text.copy(buf, text.text.size());

	InputText("Content", buf, 4096);
	InputFloat("Font size", &text.font_size);
	ColorEdit4("Font color", value_ptr(text.color));

	auto asset_manager = assets::AssetManager::Instance();
	std::vector<assets::AssetHandle> font_handles = asset_manager.GetAll(assets::kFont);
	if (font_handles.size() > 0) {
		std::vector<const char*> font_names =
				Map(font_handles, [&](auto handle) { return asset_manager.Metadata(handle)->name.c_str(); });
		int font_idx = std::find(font_handles.begin(), font_handles.end(), text.font) - font_handles.begin();

		// Font selection works because font_handles & font_names have the same name
		if (Combo("Font", &font_idx, font_names[0], font_names.size())) {
			text.font = font_handles[font_idx];
		}
	}
	if (icons::Button(icons::Id::kFile, "Upload a font into the project")) {
		BR_CORE_ASSERT(NFD::Init(), "Failed to initialize the file picker");

		NFD::UniquePath out_path;
		nfdu8filteritem_t filter_list[]{{"Font format", "ttf"}};
		nfdresult_t result = NFD::OpenDialog(out_path, filter_list, 1);
		NFD_Quit();

		if (result != NFD_OKAY) {
			BR_APP_INFO("User did not pick a file.");
			return;
		}

		std::filesystem::path font_file = out_path.get();

		if (!paths::InAssetDirectory(font_file)) {
			BR_APP_ERROR("You must pick a file inside the Asset directory");
			return;
		}

		// We don't check if LoadFont returns an empty optional, because we already guarantee that the file exists.
		assets::AssetHandle handle = context.asset_manager.LoadFont(font_file, 30).value();
		BR_APP_INFO("Added font {} to entity {}", font_file.generic_string(), static_cast<u64>(entity));
		text.font = handle;
	}
}


// ----------------------------------------------------------------
// Registration
// ----------------------------------------------------------------

/// Fills in has/add/remove generically; only 'draw' is ever written by hand.
template<typename T>
void Register(std::vector<ComponentMeta>& out, const char* name, void (*draw)(EditorContext&, Scene&, entt::entity)) {
	auto add_function = +[](Scene& scene, const entt::entity e) {
		if (scene.CanAdd<T>(e))
			scene.AddComponent<T>(e);
	};
	auto remove_function = +[](Scene& scene, const entt::entity e) {
		if (scene.CanRemove<T>(e))
			scene.RemoveComponent<T>(e);
	};

	out.push_back({name, [](Scene& scene, const entt::entity e) { return scene.reg.all_of<T>(e); }, draw, add_function,
				   remove_function});
}

std::vector<ComponentMeta> Build() {
	std::vector<ComponentMeta> components;

	// Tag, Transform and Hierarchy are attached by Scene::CreateEntity and assumed everywhere,
	// so they are shown but can neither be added nor removed.
	Register<TagComponent>(components, "Tag", DrawTag);
	Register<TransformComponent>(components, "Transform", DrawTransform);
	Register<HierarchyComponent>(components, "Hierarchy", DrawHierarchy);
	Register<VisibilityComponent>(components, "Visibility", DrawVisibility);

	// A mesh without vertices or a material cannot be drawn, so it is built by a loader or a
	// factory rather than added from the menu.
	Register<MeshMaterialComponent>(components, "Mesh", DrawMesh);

	Register<PointLightComponent>(components, "Light", DrawPointLight);

	Register<CameraComponent>(components, "Camera", DrawCamera);

	Register<ScriptComponent>(components, "Script", DrawScript);

	Register<CanvasComponent>(components, "2D Canvas", DrawCanvas);
	Register<RectTransformComponent>(components, "2D Transform", DrawRectTransform);
	Register<RectTransformComponent>(components, "2D Text", DrawText2d);

	return components;
}
} // namespace

const std::vector<ComponentMeta>& component_registry::All() {
	static const std::vector<ComponentMeta> components = Build();
	return components;
}

void component_registry::InvalidateEulerCache() { s_euler_cache_owner = entt::null; }
} // namespace bron::editor
