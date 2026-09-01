#ifndef ___PATHS_HEADER___
#define ___PATHS_HEADER___

#include <filesystem>
#include <string>

#include "Steve/Core/Core.h"

namespace Steve
{
	// Asset locations must never depend on the process working directory: every
	// IDE picks a different one (CLion uses the build dir, Visual Studio the
	// project dir, a bare ./LuigiMaker whatever the shell happens to be in).
	// The roots below are baked in by CMake and can be overridden at runtime
	// with the STEVE_ENGINE_ASSETS / STEVE_PROJECT_ASSETS environment variables.
	namespace Paths
	{
		// <repo>/Steve/Assets - shaders and other engine-owned files.
		const std::filesystem::path& EngineAssetRoot();

		// <repo>/Assets - models, textures and icons owned by the application.
		const std::filesystem::path& ProjectAssetRoot();

		std::filesystem::path EngineAsset(const std::filesystem::path& relative);
		std::filesystem::path ProjectAsset(const std::filesystem::path& relative);

		// Convenience wrappers for the many APIs that still take a std::string.
		std::string EngineAssetString(const std::filesystem::path& relative);
		std::string ProjectAssetString(const std::filesystem::path& relative);
	}
}

#endif // ___PATHS_HEADER___
