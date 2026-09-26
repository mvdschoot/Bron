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

/// Batched 2D quads. Draws are collected until EndScene, a shader switch or a full batch,
/// then submitted in one draw call - so draw order is preserved across shaders.
class BR_API R2D {
public:
	static void Init();

	static void BeginScene(const CameraView& view);
	/// Draws in pixels over the whole target, (0, 0) being its bottom-left corner.
	static void BeginScene(glm::vec2 target_size);
	static void EndScene();

	static void DrawQuad(glm::vec2 position, glm::vec2 size, const glm::vec4& color);
	static void DrawQuad(glm::vec2 position, glm::vec2 size, const Ref<Texture>& texture);
	/// 'uv_rect' is the sampled region of the texture as {x, y, width, height} in UV space.
	static void DrawQuad(glm::vec2 position, glm::vec2 size, const Ref<Texture>& texture, const glm::vec4& uv_rect,
						 const glm::vec4& tint = glm::vec4(1.0f));

	/// 'position' is the start of the baseline; '\n' moves down one line. 'scale' is relative
	/// to the size the font was imported at.
	static void DrawText(std::string_view text, const assets::AssetHandle font, glm::vec2 position, float font_size,
						 const glm::vec4& color);

private:
	static void Flush();
	static float TextureSlot(const Ref<Texture>& texture);
	static void PushQuad(glm::vec2 position, glm::vec2 size, const glm::vec4& color, float texture_slot,
						 glm::vec2 uv_min, glm::vec2 uv_max);
};
} // namespace bron
