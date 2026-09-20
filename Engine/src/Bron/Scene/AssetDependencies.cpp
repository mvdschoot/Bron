#include "Bron/Scene/AssetDependencies.h"

#include <algorithm>
#include <set>

namespace bron {
std::vector<std::filesystem::path> CollectAssets(const Scene& scene) {
	// Ordered and deduplicated: two entities sharing a model is the normal case, and an
	// exporter copying the same file twice is at best wasted work.
	std::set<std::filesystem::path> found;

	for (const auto [entity, model]: scene.reg.view<const ModelSourceComponent>().each()) {
		if (!model.path.empty())
			found.insert(std::filesystem::path(model.path).lexically_normal());
	}

	// Every other component that comes to carry an asset path belongs here, next to this
	// one. A ScriptComponent is the next of them.

	return {found.begin(), found.end()};
}
} // namespace bron
