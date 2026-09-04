#include "Core/Preferences.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>

#include <nlohmann/json.hpp>

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

namespace bron::editor {
namespace {
using json = nlohmann::json;

std::filesystem::path EnvPath(const char* name) {
	// std::getenv is deprecated-but-portable; getenv_s/secure_getenv are not, so we
	// keep the standard one and silence MSVC in CMake (same as bron::Paths).
	const char* value = std::getenv(name);
	return value && *value != '\0' ? std::filesystem::path(value) : std::filesystem::path();
}

/// The OS location for per-user application configuration.
std::filesystem::path ConfigDirectory() {
#if defined(BR_PLATFORM_WINDOWS)
	if (std::filesystem::path app_data = EnvPath("APPDATA"); !app_data.empty())
		return app_data;
#elif defined(BR_PLATFORM_MACOS)
	if (std::filesystem::path home = EnvPath("HOME"); !home.empty())
		return home / "Library" / "Application Support";
#elif defined(BR_PLATFORM_LINUX)
	if (std::filesystem::path xdg = EnvPath("XDG_CONFIG_HOME"); !xdg.empty())
		return xdg;
	if (std::filesystem::path home = EnvPath("HOME"); !home.empty())
		return home / ".config";
#endif

	// No home directory to speak of - keep the editor working out of the working
	// directory rather than failing to start.
	BR_CORE_WARN("No user config directory found; preferences will be kept next to the executable.");
	return std::filesystem::current_path();
}

/// Reads one value, leaving 'out' untouched when the key is absent or the wrong type.
/// Missing keys are normal: a file written by an older build will not have the newer ones.
template<typename T>
void ReadInto(const json& object, const char* key, T& out) {
	const auto it = object.find(key);
	if (it == object.end())
		return;

	try {
		out = it->get<T>();
	} catch (const json::exception& e) {
		BR_CORE_WARN("Preference '{}' has the wrong type, keeping the default: {}", key, e.what());
	}
}
} // namespace

Preferences& Preferences::Get() {
	static Preferences preferences;
	return preferences;
}

const std::filesystem::path& Preferences::File() {
	static const std::filesystem::path file = ConfigDirectory() / "Bron" / "preferences.json";
	return file;
}

void Preferences::Load() {
	Preferences& prefs = Get();

	std::ifstream stream(File());
	if (!stream) {
		// First run: keep the defaults and write them out, so the file is there to be edited.
		BR_CORE_INFO("No preferences at {}, writing the defaults.", File().string());
		Save();
		return;
	}

	json root;
	try {
		stream >> root;
	} catch (const json::exception& e) {
		// A corrupt file must not stop the editor from starting; the defaults are always usable.
		BR_CORE_ERROR("Could not parse {}, falling back to the default preferences: {}", File().string(), e.what());
		return;
	}

	if (const auto appearance = root.find("appearance"); appearance != root.end()) {
		ReadInto(*appearance, "theme", prefs.theme);
		ReadInto(*appearance, "uiScale", prefs.ui_scale);
	}

	if (const auto camera = root.find("camera"); camera != root.end()) {
		ReadInto(*camera, "orbitSpeed", prefs.camera_orbit_speed);
		ReadInto(*camera, "zoomSpeed", prefs.camera_zoom_speed);
	}

	std::vector<std::string> recent;
	ReadInto(root, "recentProjects", recent);

	prefs.recent_projects.clear();
	for (const std::string& path: recent)
		prefs.recent_projects.emplace_back(path);
}

void Preferences::Save() {
	const Preferences& prefs = Get();

	json root;
	root["appearance"]["theme"] = prefs.theme;
	root["appearance"]["uiScale"] = prefs.ui_scale;
	root["camera"]["orbitSpeed"] = prefs.camera_orbit_speed;
	root["camera"]["zoomSpeed"] = prefs.camera_zoom_speed;

	json recent = json::array();
	for (const std::filesystem::path& path: prefs.recent_projects) {
		if (recent.size() >= kMaxRecentProjects)
			break;

		// generic_string keeps the file readable and portable across platforms.
		recent.push_back(path.generic_string());
	}
	root["recentProjects"] = std::move(recent);

	std::error_code error;
	std::filesystem::create_directories(File().parent_path(), error);
	if (error) {
		BR_CORE_ERROR("Could not create {}: {}", File().parent_path().string(), error.message());
		return;
	}

	std::ofstream stream(File());
	if (!stream) {
		BR_CORE_ERROR("Could not write {}", File().string());
		return;
	}

	stream << root.dump(1, '\t') << '\n';
}

void Preferences::AddRecentProject(const std::filesystem::path& path) {
	std::vector<std::filesystem::path>& recent = Get().recent_projects;

	const auto existing = std::find(recent.begin(), recent.end(), path);
	if (existing != recent.end())
		recent.erase(existing);

	recent.insert(recent.begin(), path);

	if (recent.size() > kMaxRecentProjects)
		recent.resize(kMaxRecentProjects);
}
} // namespace bron::editor
