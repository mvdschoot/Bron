#include "Paths.h"

#include <cstdlib>

#include "Steve/Core/Logger.h"

#ifndef STEVE_ENGINE_ASSET_DIR
	#error "STEVE_ENGINE_ASSET_DIR is not defined - configure the project through CMake."
#endif

#ifndef STEVE_PROJECT_ASSET_DIR
	#error "STEVE_PROJECT_ASSET_DIR is not defined - configure the project through CMake."
#endif

namespace Steve
{
	namespace Paths
	{
		namespace
		{
			std::filesystem::path Resolve(const char* env_var, const char* compiled_in)
			{
				// std::getenv is deprecated-but-portable; getenv_s/secure_getenv
				// are not, so we keep the standard one and silence MSVC in CMake.
				if (const char* override_dir = std::getenv(env_var))
				{
					if (*override_dir != '\0')
						return std::filesystem::path(override_dir);
				}

				return std::filesystem::path(compiled_in);
			}
		}

		const std::filesystem::path& EngineAssetRoot()
		{
			static const std::filesystem::path root = Resolve("STEVE_ENGINE_ASSETS", STEVE_ENGINE_ASSET_DIR);
			return root;
		}

		const std::filesystem::path& ProjectAssetRoot()
		{
			static const std::filesystem::path root = Resolve("STEVE_PROJECT_ASSETS", STEVE_PROJECT_ASSET_DIR);
			return root;
		}

		std::filesystem::path EngineAsset(const std::filesystem::path& relative)
		{
			return EngineAssetRoot() / relative;
		}

		std::filesystem::path ProjectAsset(const std::filesystem::path& relative)
		{
			return ProjectAssetRoot() / relative;
		}

		std::string EngineAssetString(const std::filesystem::path& relative)
		{
			return EngineAsset(relative).string();
		}

		std::string ProjectAssetString(const std::filesystem::path& relative)
		{
			return ProjectAsset(relative).string();
		}
	}
}
