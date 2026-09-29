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

void SceneRenderer::Draw(Scene& scene, const CameraView& view, const glm::vec2 target_size) {
	BR_PROFILE_FUNCTION();

	DrawWorld(scene, view);
	DrawScreen(scene, target_size);
}

void SceneRenderer::DrawWorld(Scene& scene, const CameraView& view) {
	WorldRenderer::Draw(scene, view);
	CanvasRenderer::DrawWorld(scene, view);
}

void SceneRenderer::DrawScreen(Scene& scene, const glm::vec2 target_size) {
	CanvasRenderer::DrawScreen(scene, target_size);
}
} // namespace bron
