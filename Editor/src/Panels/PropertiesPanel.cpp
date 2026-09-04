#include "Panels/PropertiesPanel.h"

#include "Panels/ComponentRegistry.h"

namespace bron::editor {
using namespace ImGui;

void PropertiesPanel::OnImGuiRender() {
	Begin("Properties");

	if (context_.HasSelection()) {
		DrawComponents(context_.selection);

		Separator();

		if (Button("Add Component"))
			OpenPopup("AddComponent");

		DrawAddComponentMenu(context_.selection);
	}

	End();
}

void PropertiesPanel::DrawComponents(const entt::entity entity) {
	Scene& scene = context_.scene;

	for (const ComponentMeta& component: component_registry::All()) {
		if (!component.has(scene, entity))
			continue;

		PushID(component.name);

		// AllowOverlap lets the remove button below claim clicks in the part of the header row it
		// covers; without it the header is submitted first and swallows them into a collapse toggle.
		const bool open =
				CollapsingHeader(component.name, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);

		// Remove button on the header row itself, so a collapsed section can still be removed.
		bool remove_requested = false;
		if (component.remove) {
			SameLine(GetWindowWidth() - 30.0f);
			remove_requested = SmallButton("x");
		}

		if (open)
			component.draw(scene, entity);

		PopID();

		// Applied after drawing, so this frame's draw still sees a live component.
		if (remove_requested)
			component.remove(scene, entity);
	}
}

void PropertiesPanel::DrawAddComponentMenu(const entt::entity entity) {
	if (!BeginPopup("AddComponent"))
		return;

	Scene& scene = context_.scene;

	bool any_offered = false;
	for (const ComponentMeta& component: component_registry::All()) {
		if (!component.add || component.has(scene, entity))
			continue;

		any_offered = true;
		if (MenuItem(component.name))
			component.add(scene, entity);
	}

	if (!any_offered)
		TextDisabled("Nothing left to add");

	EndPopup();
}
} // namespace bron::editor
