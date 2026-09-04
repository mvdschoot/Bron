#pragma once

#include <filesystem>
#include <string>

#include "Bron/Core/Core.h"

namespace Bron
{
	// Asset locations must never depend on the process working directory: every
	// IDE picks a different one (CLion uses the build dir, Visual Studio the
	// project dir, a bare ./BronEditor whatever the shell happens to be in).
	// The root below is baked in by CMake and can be overridden at runtime with
	// the BRON_PROJECT_ASSETS environment variable.
	//
	// There is deliberately no engine asset root: the engine's own shaders are
	// compiled into the binary (see Bron/Graphics/BuiltinShaders.h), so assets
	// belong to the application alone.
	namespace Paths
	{
		// <repo>/Assets - models, textures and icons owned by the application.
		const std::filesystem::path& ProjectAssetRoot();

		std::filesystem::path ProjectAsset(const std::filesystem::path& relative);

		// Convenience wrapper for the many APIs that still take a std::string.
		std::string ProjectAssetString(const std::filesystem::path& relative);


	}
}
