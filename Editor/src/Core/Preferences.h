#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace bron::editor {
/// Settings that belong to the person using the editor rather than to anything they
/// are editing: they follow the install, not the project, and are stored in the user's
/// OS config directory. Anything that describes a project belongs in Project instead,
/// and anything that describes one machine's view of a project belongs in its session.
///
/// Deliberately not a layered/override system: every setting is owned by exactly one
/// scope, so there is nothing to resolve at read time.
struct Preferences {
	// --- Appearance ---

	/// Name of a palette registered in Theme.cpp. Falls back to the default when unknown.
	std::string theme = "Dark";

	/// Multiplies every style metric and the font, for high-DPI displays.
	float ui_scale = 1.0f;

	// --- Viewport navigation ---

	/// Radians per second of held orbit input.
	float camera_orbit_speed = 1.0f;

	/// World units per scroll notch.
	float camera_zoom_speed = 0.15f;

	// --- Projects ---

	/// Most recently opened first. Truncated to kMaxRecentProjects on write.
	std::vector<std::filesystem::path> recent_projects;

	static constexpr size_t kMaxRecentProjects = 10;


	/// The single instance. Safe to call before Load(), which gives the defaults above.
	static Preferences& Get();

	/// Reads the preferences file, keeping the default for anything missing or malformed.
	/// Call before the window is created - the theme and the scale are needed at startup.
	static void Load();

	/// Writes the preferences file, creating the config directory if needed.
	static void Save();

	/// Moves 'path' to the front of the recent list, without duplicating it.
	static void AddRecentProject(const std::filesystem::path& path);

	/// <config>/Bron/preferences.json - %APPDATA% on Windows, XDG_CONFIG_HOME elsewhere.
	static const std::filesystem::path& File();
};
} // namespace bron::editor
