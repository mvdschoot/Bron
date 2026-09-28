#include "SceneRenderer.h"

#include "2D.h"
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

	R2D::BeginScene(target_size);
	for (const auto [entity, text]: scene.reg.view<Text2DComponent>().each()) {
		if (scene.IsVisible(entity))
			R2D::DrawText(text.text, text.font, scene.ScreenTransform(entity).min, text.font_size, text.color);
	}
	R2D::EndScene();
}
} // namespace bron
