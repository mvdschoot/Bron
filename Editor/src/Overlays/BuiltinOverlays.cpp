#include "Overlays/BuiltinOverlays.h"

#include "Core/DefaultAssets.h"

#include "Bron/Graphics/Renderer/2D.h"
#include "Bron/Graphics/Renderer/Command.h"
#include "Bron/Graphics/Renderer/Grid.h"
#include "Bron/Graphics/Renderer/WorldRenderer.h"

#include <vector>

namespace bron::editor {
namespace {
// What is selected is not always what is drawn: a loaded model is a parent entity whose
// meshes hang off it as children, and only the children carry geometry. Outlining a
// selection means outlining every mesh underneath it.
void CollectMeshes(Scene& scene, const entt::entity entity, std::vector<entt::entity>& out) {
	if (entity == entt::null)
		return;

	if (scene.reg.all_of<MeshMaterialComponent>(entity))
		out.push_back(entity);

	if (const HierarchyComponent* hierarchy = scene.reg.try_get<HierarchyComponent>(entity)) {
		for (const entt::entity child: hierarchy->children)
			CollectMeshes(scene, child, out);
	}
}
} // namespace

void GridOverlay::DrawWorld(const OverlayContext& context) {
	if (context.editor.state != kEdit)
		return;

	// The grid fades out towards its edges, so it is blended over the world. It writes the
	// depth of the floor it hits, which is what lets the world hide it.
	Command::EnableBlend();
	GridRenderer::Draw(context.view);
	Command::EnableDepth();
}

void SelectionOutlineOverlay::DrawWorld(const OverlayContext& context) {
	// The selection is an entity of the edited scene, so it only means something there.
	if (context.playing || context.scene == nullptr)
		return;

	std::vector<entt::entity> meshes;
	CollectMeshes(*context.scene, context.editor.selection, meshes);
	if (!meshes.empty())
		WorldRenderer::DrawOutline(*context.scene, context.view, meshes);
}

void DebugTextOverlay::DrawScreen(const OverlayContext& context) {}
} // namespace bron::editor
