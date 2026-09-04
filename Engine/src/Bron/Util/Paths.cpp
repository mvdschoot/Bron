#include "Paths.h"

#include "Bron/Core/Logger.h"

namespace bron {
namespace paths {
namespace {
struct Roots {
	std::filesystem::path project;
	std::filesystem::path assets;
};

Roots& Current() {
	static Roots roots;
	return roots;
}
} // namespace

const std::filesystem::path& ProjectRoot() { return Current().project; }

const std::filesystem::path& AssetRoot() { return Current().assets; }

bool HasRoots() { return !Current().project.empty() && !Current().assets.empty(); }

void SetRoots(const std::filesystem::path& project, const std::filesystem::path& assets) {
	BR_CORE_INFO("Project root is now {}, assets {}", project.string(), assets.string());

	Current().project = project;
	Current().assets = assets;
}

std::filesystem::path ResolveProject(const std::filesystem::path& relative) {
	// Reachable only with a project open, which is what sets the roots - so this firing
	// means something resolved a path without one.
	BR_CORE_ASSERT(HasRoots(), "No roots: resolving a project path with no project open");

	return ProjectRoot() / relative;
}

std::filesystem::path ResolveAsset(const std::filesystem::path& relative) {
	BR_CORE_ASSERT(HasRoots(), "No roots: resolving an asset with no project open");

	return AssetRoot() / relative;
}

std::filesystem::path RelativeToAsset(const std::filesystem::path& absolute) {
	BR_CORE_ASSERT(HasRoots(), "No roots: storing an asset path with no project open");

	std::error_code ec;
	const std::filesystem::path relative = std::filesystem::relative(absolute, AssetRoot(), ec);

	const bool inside = !ec && !relative.empty() && *relative.begin() != "..";
	return inside ? relative : absolute;
}
} // namespace paths
} // namespace bron
