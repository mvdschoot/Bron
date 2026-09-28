#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Scene/Asset.h"

#include "glm/glm.hpp"

#include <string_view>

namespace bron {
struct CameraView;
namespace assets {
struct FontAsset;
}

/// Immediate-mode 2D drawing: quads and text, in whatever space the view passed to
/// BeginScene looks at. Knows nothing about scenes; CanvasRenderer walks those and draws
/// through this.
///
/// Draws are batched: collected until EndScene, a full batch or running out of texture
/// slots, then submitted in one draw call. Batches are only ever cut, never reordered, so
/// what is drawn later always lands on top.
class BR_API R2D {
public:
	static void Init();

	static void BeginScene(const CameraView& view);
	/// Draws in pixels over the whole target, (0, 0) being its bottom-left corner.
	static void BeginScene(glm::vec2 target_size);
	static void EndScene();

	/// 'transform' places the unit square: (0, 0) to (1, 1) lands wherever it maps them.
	static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
	/// 'uv_rect' is the sampled region of the texture as {x, y, width, height} in UV space.
	static void DrawQuad(const glm::mat4& transform, const Ref<Texture>& texture,
						 const glm::vec4& uv_rect = {0.0f, 0.0f, 1.0f, 1.0f}, const glm::vec4& tint = glm::vec4(1.0f));

	static void DrawQuad(glm::vec2 position, glm::vec2 size, const glm::vec4& color);
	static void DrawQuad(glm::vec2 position, glm::vec2 size, const Ref<Texture>& texture,
						 const glm::vec4& uv_rect = {0.0f, 0.0f, 1.0f, 1.0f}, const glm::vec4& tint = glm::vec4(1.0f));

	/// Lays the text out in the space 'transform' maps from: the first line's baseline starts
	/// at the origin and runs along +x, and every '\n' moves down one line. 'font_size' is in
	/// that same space.
	static void DrawText(std::string_view text, const assets::AssetHandle& font, float font_size,
						 const glm::vec4& color, const glm::mat4& transform);
	/// As above, with the first baseline starting at 'position'.
	static void DrawText(std::string_view text, const assets::AssetHandle& font, glm::vec2 position, float font_size,
						 const glm::vec4& color);

private:
	static void Flush();
	static float TextureSlot(const Ref<Texture>& texture);
	static void PushQuad(const glm::mat4& transform, const glm::vec4& color, float texture_slot, glm::vec2 uv_min,
						 glm::vec2 uv_max);
};
} // namespace bron
