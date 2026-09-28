#pragma once

#include <vector>

#include "Bron.h"

#include "Core/EditorContext.h"

namespace bron::editor {
/// Everything the editor needs to know about one component type, with the type erased into
/// plain function pointers so that all component types fit in a single list.
struct ComponentMeta {
	const char* name;

	bool (*has)(Scene&, entt::entity);
	/// Takes the context as well as the scene: a few components have editor-only state that
	/// lives outside the component (the camera preview toggle), and the rest ignore it.
	void (*draw)(EditorContext&, Scene&, entt::entity);

	// Null when scene.CanAdd/Remove is false.
	void (*add)(Scene&, entt::entity);
	void (*remove)(Scene&, entt::entity);
};

namespace component_registry {
/// Every component the editor knows about, in inspector display order.
const std::vector<ComponentMeta>& All();

/// Euler angles are only a UI view of the transform's quaternion, so the panel caches them.
/// Call this after changing a rotation from outside the inspector (the gizmo).
void InvalidateEulerCache();
} // namespace component_registry
} // namespace bron::editor
