#include "SceneRenderer.h"

#include "2D.h"
#include "CanvasRenderer.h"
#include "Command.h"
#include "WorldRenderer.h"

namespace bron {
void SceneRenderer::Init() {
	WorldRenderer::Init();
	R2D::Init();
}

void SceneRenderer::Draw(Scene& scene, const CameraView& view, const glm::vec2 target_size,
						 const std::vector<entt::entity>& outlined) {
	BR_PROFILE_FUNCTION();

	WorldRenderer::Draw(scene, view);
	WorldRenderer::DrawOutline(scene, view, outlined);

	CanvasRenderer::DrawWorld(scene, view);
	CanvasRenderer::DrawScreen(scene, target_size);
}
} // namespace bron
