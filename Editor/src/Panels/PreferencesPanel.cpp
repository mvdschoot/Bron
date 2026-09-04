#include "Panels/PreferencesPanel.h"

#include "Core/Preferences.h"
#include "Core/Theme.h"

namespace Bron::Editor
{
	using namespace ImGui;

	void PreferencesPanel::OnImGuiRender()
	{
		if (!mOpen)
			return;

		if (!Begin("Preferences", &mOpen))
		{
			End();
			return;
		}

		Preferences& prefs = Preferences::Get();

		// Restyling mid-frame would leave the windows already submitted this frame using the
		// old style, so the theme change is applied at the end of the function instead.
		bool restyle = false;

		if (CollapsingHeader("Appearance", ImGuiTreeNodeFlags_DefaultOpen))
		{
			Indent();

			if (BeginCombo("Theme", prefs.theme.c_str()))
			{
				for (const char* name : Theme::Names())
				{
					if (Selectable(name, prefs.theme == name))
					{
						prefs.theme = name;
						restyle = true;
					}
				}
				EndCombo();
			}

			restyle |= SliderFloat("UI scale", &prefs.uiScale, 0.5f, 3.0f, "%.2f");

			Unindent();
		}

		if (CollapsingHeader("Viewport navigation", ImGuiTreeNodeFlags_DefaultOpen))
		{
			Indent();
			DragFloat("Orbit speed", &prefs.cameraOrbitSpeed, 0.05f, 0.05f, 10.0f, "%.2f rad/s");
			DragFloat("Zoom speed", &prefs.cameraZoomSpeed, 0.01f, 0.01f, 2.0f, "%.2f");
			Unindent();
		}

		Separator();

		if (Button("Save"))
			Preferences::Save();

		SameLine();
		TextDisabled("%s", Preferences::File().string().c_str());

		End();

		if (restyle)
			Theme::Apply();
	}
}
