#pragma once

#include <vector>

#include "Bron.h"

namespace bron::editor {
enum ComponentFlags {
	kComponentFlagsNone = 0,

	// Offered in the "Add Component" menu. Off for components that cannot be usefully
	// default constructed, and for the ones every entity is guaranteed to have.
	kComponentFlagsAddable = BR_BIT(0),

	// Gets a remove button in the inspector. Off for the structural components, because
	// the rest of the engine assumes every entity has them.
	kComponentFlagsRemovable = BR_BIT(1),

	kComponentFlagsDefault = kComponentFlagsAddable | kComponentFlagsRemovable
};

/// Everything the editor needs to know about one component type, with the type erased into
/// plain function pointers so that all component types fit in a single list.
struct ComponentMeta {
	const char* name;

	bool (*has)(Scene&, entt::entity);
	void (*draw)(Scene&, entt::entity);

	// Null when the component carries the matching flag, meaning the UI does not offer the action.
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
