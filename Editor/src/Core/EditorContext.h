#pragma once

#include "Bron.h"
#include "ImGuizmo.h"

#include "Core/EditorCamera.h"

namespace Bron::Editor
{
	/// The state every panel shares: what is being edited, what is selected, and how the
	/// viewport looks at it. Panels hold a reference to this instead of reaching into each
	/// other, so a panel can be added or removed without touching the rest.
	struct EditorContext
	{
		EditorContext()
		{
			scene.camera = &camera;
		}

		Scene scene;
		EditorCamera camera{glm::radians(80.0f), 16.0f / 9.0f, 0.1f, 100.0f};

		/// The entity the inspector and the gizmo act on, or entt::null for "nothing selected".
		entt::entity selection = entt::null;
		ImGuizmo::OPERATION gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

		/// Duration of the last frame, for the statistics panel.
		Timestep frameTime;

		[[nodiscard]] bool HasSelection() const { return selection != entt::null; }
		void ClearSelection() { selection = entt::null; }
	};
}
