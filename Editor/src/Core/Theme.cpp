#include "Core/Theme.h"

#include <cstring>

#include "imgui.h"

#include "Core/Preferences.h"
#include "Bron/Core/Logger.h"

namespace Bron::Editor
{
	namespace
	{
		/// Metrics are shared by every palette; only the colours differ. ScaleAllSizes() below
		/// multiplies these by the UI scale, so they are written at scale 1.
		void ApplyMetrics(ImGuiStyle& style)
		{
			style.WindowPadding = ImVec2(15, 15);
			style.WindowRounding = 5.0f;
			style.FramePadding = ImVec2(5, 5);
			style.FrameRounding = 4.0f;
			style.ItemSpacing = ImVec2(12, 8);
			style.ItemInnerSpacing = ImVec2(8, 6);
			style.IndentSpacing = 25.0f;
			style.ScrollbarSize = 15.0f;
			style.ScrollbarRounding = 9.0f;
			style.GrabMinSize = 5.0f;
			style.GrabRounding = 3.0f;
		}

		void Dark(ImVec4* colors)
		{
			colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
			colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
			colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
			colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
			colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
			colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
			colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		}

		struct Palette
		{
			const char* name;
			void (*apply)(ImVec4*);
		};

		// Adding a look is one function above and one entry here; the name is what goes into
		// the preferences file. The first entry is the fallback.
		constexpr Palette Palettes[] = {
			{"Dark", Dark},
		};
	}

	void Theme::Apply()
	{
		const Preferences& prefs = Preferences::Get();

		ImGuiStyle& style = ImGui::GetStyle();

		// Reset first: Apply() runs again whenever the preferences change, and ScaleAllSizes()
		// is relative, so it would compound on the already scaled metrics.
		style = ImGuiStyle();
		ApplyMetrics(style);

		const Palette* palette = &Palettes[0];
		for (const Palette& candidate : Palettes)
		{
			if (std::strcmp(candidate.name, prefs.theme.c_str()) == 0)
			{
				palette = &candidate;
				break;
			}
		}

		if (std::strcmp(palette->name, prefs.theme.c_str()) != 0)
			CORE_WARN("Unknown theme '{}', using '{}'.", prefs.theme, palette->name);

		palette->apply(style.Colors);

		style.ScaleAllSizes(prefs.uiScale);
		ImGui::GetIO().FontGlobalScale = prefs.uiScale;
	}

	const std::vector<const char*>& Theme::Names()
	{
		static const std::vector<const char*> names = []
		{
			std::vector<const char*> result;
			for (const Palette& palette : Palettes)
				result.push_back(palette.name);
			return result;
		}();

		return names;
	}
}
