#include "Paths.h"

#include "Bron/Core/Logger.h"

namespace bron
{
	namespace paths
	{
		namespace
		{
			std::filesystem::path& Root()
			{
				static std::filesystem::path root;
				return root;
			}
		}

		const std::filesystem::path& AssetRoot()
		{
			return Root();
		}

		bool HasAssetRoot()
		{
			return !Root().empty();
		}

		void SetAssetRoot(const std::filesystem::path& root)
		{
			BR_CORE_INFO("Asset root is now {}", root.string());
			Root() = root;
		}

		std::filesystem::path Resolve(const std::filesystem::path& relative)
		{
			// Reachable only through a loaded scene, which needs a project, which sets the
			// root - so this means something resolved an asset path without one.
			BR_CORE_ASSERT(HasAssetRoot(), "No asset root: resolving an asset with no project open");

			return AssetRoot() / relative;
		}

		std::string ResolveString(const std::filesystem::path& relative)
		{
			return Resolve(relative).string();
		}

		std::filesystem::path Relative(const std::filesystem::path& absolute)
		{
			BR_CORE_ASSERT(HasAssetRoot(), "No asset root: storing an asset path with no project open");

			std::error_code ec;
			const std::filesystem::path relative = std::filesystem::relative(absolute, AssetRoot(), ec);

			const bool inside = !ec && !relative.empty() && *relative.begin() != "..";
			return inside ? relative : absolute;
		}
	}
}
