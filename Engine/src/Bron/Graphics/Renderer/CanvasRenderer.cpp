#include "CanvasRenderer.h"

#include "2D.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <vector>

namespace bron {
namespace {
// 'parent_rect' is in the space 'parent_transform' maps from - the parent's own, before
// its scale and rotation - which is the space this entity's anchors are measured in.
void DrawNode(Scene& scene, const entt::entity entity, const Box2D& parent_rect, const glm::mat4& parent_transform) {
	if (!scene.reg.get<VisibilityComponent>(entity).visible)
		return;

	// Something that is not 2D, hung under a canvas. There is no rect to lay its children
	// out against, so they are not drawn either.
	const RectTransformComponent* rect_transform = scene.reg.try_get<RectTransformComponent>(entity);
	if (rect_transform == nullptr)
		return;

	const Box2D rect = rect_transform->Rect(parent_rect);
	const glm::mat4 transform = parent_transform * rect_transform->Local(rect);

	if (const Box2DComponent* box = scene.reg.try_get<Box2DComponent>(entity)) {
		const glm::mat4 placement =
				glm::scale(glm::translate(transform, glm::vec3(rect.min, 0.0f)), glm::vec3(rect.max - rect.min, 1.0f));
		R2D::DrawQuad(placement, box->color);
	}

	// Starts in the top-left corner: the first baseline sits one line below the top.
	if (const Text2DComponent* text = scene.reg.try_get<Text2DComponent>(entity)) {
		const glm::vec3 start(rect.min.x, rect.max.y - text->font_size, 0.0f);
		R2D::DrawText(text->text, text->font, text->font_size, text->color, glm::translate(transform, start));
	}

	for (const entt::entity child: scene.reg.get<HierarchyComponent>(entity).children)
		DrawNode(scene, child, rect, transform);
}

// 'size' is the canvas's own rect, in canvas pixels; 'transform' maps those pixels to
// wherever the canvas is drawn.
void DrawCanvas(Scene& scene, const entt::entity canvas, const glm::vec2 size, const glm::mat4& transform) {
	const Box2D root{.min = {0.0f, 0.0f}, .max = size};

	for (const entt::entity child: scene.reg.get<HierarchyComponent>(canvas).children)
		DrawNode(scene, child, root, transform);
}
} // namespace

void CanvasRenderer::DrawWorld(Scene& scene, const CameraView& view) {
	BR_PROFILE_FUNCTION();

	R2D::BeginScene(view);
	for (const auto [entity, canvas]: scene.reg.view<const CanvasComponent>().each()) {
		if (canvas.mode != CanvasComponent::Mode::kWorldSpace || !scene.IsVisible(entity))
			continue;

		const glm::mat4 pixels_to_world =
				glm::scale(scene.WorldTransform(entity), glm::vec3(1.0f / canvas.pixels_per_unit));
		DrawCanvas(scene, entity, canvas.reference_size, pixels_to_world);
	}
	R2D::EndScene();
}

void CanvasRenderer::DrawScreen(Scene& scene, const glm::vec2 target_size) {
	BR_PROFILE_FUNCTION();

	std::vector<entt::entity> canvases;
	for (const auto [entity, canvas]: scene.reg.view<const CanvasComponent>().each()) {
		if (canvas.mode == CanvasComponent::Mode::kScreenSpace && scene.IsVisible(entity))
			canvases.push_back(entity);
	}

	// Stable, so canvases sharing a sort_order keep one order from frame to frame.
	std::ranges::stable_sort(canvases, {},
							 [&](const entt::entity e) { return scene.reg.get<CanvasComponent>(e).sort_order; });

	R2D::BeginScene(target_size);
	for (const entt::entity entity: canvases) {
		const CanvasComponent& canvas = scene.reg.get<CanvasComponent>(entity);

		// Scaled so the reference height fills the target: a canvas pixel is a screen pixel
		// at the reference size and grows or shrinks with the target from there. The width
		// follows the target's shape rather than the reference's, so anchors always reach
		// the real edges of the screen whatever its aspect.
		const float scale = canvas.reference_size.y > 0.0f ? target_size.y / canvas.reference_size.y : 1.0f;
		if (scale <= 0.0f)
			continue;

		DrawCanvas(scene, entity, target_size / scale, glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0f)));
	}
	R2D::EndScene();
}
} // namespace bron
