#include "Panels/PropertiesPanel.h"

#include "Panels/ComponentRegistry.h"

namespace Bron::Editor
{
	using namespace ImGui;

	void PropertiesPanel::OnImGuiRender()
	{
		Begin("Properties");

		if (Context.HasSelection())
		{
			DrawComponents(Context.selection);

			Separator();

			if (Button("Add Component"))
				OpenPopup("AddComponent");

			DrawAddComponentMenu(Context.selection);
		}

		End();
	}

	void PropertiesPanel::DrawComponents(const entt::entity entity)
	{
		Scene& scene = Context.scene;

		for (const ComponentMeta& component : ComponentRegistry::All())
		{
			if (!component.has(scene, entity))
				continue;

			PushID(component.name);

			// AllowOverlap lets the remove button below claim clicks in the part of the header row it
			// covers; without it the header is submitted first and swallows them into a collapse toggle.
			const bool open = CollapsingHeader(component.name,
											   ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);

			// Remove button on the header row itself, so a collapsed section can still be removed.
			bool removeRequested = false;
			if (component.remove)
			{
				SameLine(GetWindowWidth() - 30.0f);
				removeRequested = SmallButton("x");
			}

			if (open)
				component.draw(scene, entity);

			PopID();

			// Applied after drawing, so this frame's draw still sees a live component.
			if (removeRequested)
				component.remove(scene, entity);
		}
	}

	void PropertiesPanel::DrawAddComponentMenu(const entt::entity entity)
	{
		if (!BeginPopup("AddComponent"))
			return;

		Scene& scene = Context.scene;

		bool anyOffered = false;
		for (const ComponentMeta& component : ComponentRegistry::All())
		{
			if (!component.add || component.has(scene, entity))
				continue;

			anyOffered = true;
			if (MenuItem(component.name))
				component.add(scene, entity);
		}

		if (!anyOffered)
			TextDisabled("Nothing left to add");

		EndPopup();
	}
}
