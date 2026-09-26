#pragma once

#include "Bron.h"
#include "ImGuizmo.h"
#include "Bron/Scene/AssetManager.h"

#include <memory>

#include "Core/EditorCamera.h"
#include "Core/Project.h"

namespace bron::editor {
class Panel;

enum EditorState {
	kEdit,
	kPlay,
};

/// The state every panel shares: what is being edited, what is selected, and how the
/// viewport looks at it. Panels hold a reference to this instead of reaching into each
/// other, so a panel can be added or removed without touching the rest.
struct EditorContext {
	/// The open project, or null when there is none - the state the editor starts in
	/// when nothing has been opened before. Panels must handle null; with no project
	/// there is no asset root, so nothing may resolve an asset path.
	Scope<Project> project;

	[[nodiscard]] bool HasProject() const { return project != nullptr; }

	/// The scene being edited. Owned by the project, not by the context - null whenever
	/// there is no project open.
	Scene* active_scene = nullptr;

	[[nodiscard]] bool HasScene() const { return active_scene != nullptr; }

	EditorCamera camera_3d{glm::radians(80.0f), 0.1f, 100.0f};
	entt::entity active_camera = entt::null;


	/// The entity the inspector and the gizmo act on, or entt::null for "nothing selected".
	entt::entity selection = entt::null;
	ImGuizmo::OPERATION gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;


	/// Where mouse and keyboard events go. Rebuilt every ImGui frame by the panels
	/// themselves; null when the cursor or focus is on something that is not a panel.
	Panel* hovered_panel = nullptr;
	Panel* focused_panel = nullptr;

	EditorState state = kEdit;
	Ref<Scene> play_scene;

	/// Duration of the last frame, for the statistics panel.
	Timestep frame_time;

	assets::AssetManager& asset_manager = assets::AssetManager::Instance();

	[[nodiscard]] bool HasSelection() const { return selection != entt::null; }
	void ClearSelection() { selection = entt::null; }

	/// Points the editor at a scene. The old selection belonged to the old scene, so it
	/// goes.
	void SetActiveScene(Scene* scene) {
		active_scene = scene;
		ClearSelection();
		active_camera = entt::null;
	}
};
} // namespace bron::editor
