#include "2D.h"

#include "Bron/Graphics/BuiltinShaders.h"
#include "Command.h"
#include "Bron/Graphics/Camera2D.h"
#include "Bron/Scene/Asset.h"
#include "Bron/Scene/AssetManager.h"

#include <array>
#include <numeric>
#include <vector>

namespace bron {
namespace {
struct QuadVertex {
	glm::vec2 position;
	glm::vec4 color;
	glm::vec2 tex_coord;
	float tex_index;
};

constexpr u32 kMaxQuads = 20000;
constexpr u32 kMaxVertices = kMaxQuads * 4;
constexpr u32 kMaxIndices = kMaxQuads * 6;
// Has to match the size of uTextures in the 2D shaders.
constexpr u32 kMaxTextureSlots = 32;

struct R2DData {
	Ref<Shader> shader;
	Ref<VertexArray> vertex_array;
	Ref<VertexBuffer> vertex_buffer;
	std::vector<QuadVertex> vertices;

	// Slot 0 is always the white texture, so untextured quads can share a batch.
	std::array<Ref<Texture>, kMaxTextureSlots> texture_slots;
	u32 texture_count = 1;

	CameraView view;
};

R2DData render_data;
} // namespace

static const assets::FontAsset* ResolveFont(const assets::AssetHandle& font_handle) {
	return assets::AssetManager::Instance().Get<assets::FontAsset>(font_handle).get();
}

void R2D::Init() {
	BR_PROFILE_FUNCTION();

	render_data.vertex_buffer = VertexBuffer::Create(kMaxVertices * sizeof(QuadVertex));
	render_data.vertex_buffer->SetBufferLayout({{"a_Position", ShaderDataType::kFloat2},
												{"a_Color", ShaderDataType::kFloat4},
												{"a_TexCoord", ShaderDataType::kFloat2},
												{"a_TexIndex", ShaderDataType::kFloat}});

	render_data.vertex_array = VertexArray::Create();
	render_data.vertex_array->AddVertexBuffer(render_data.vertex_buffer);

	std::vector<u32> indices(kMaxIndices);
	for (u32 quad = 0; quad < kMaxQuads; ++quad) {
		const u32 v = quad * 4;
		const u32 i = quad * 6;
		indices[i + 0] = v + 0;
		indices[i + 1] = v + 1;
		indices[i + 2] = v + 2;
		indices[i + 3] = v + 2;
		indices[i + 4] = v + 3;
		indices[i + 5] = v + 0;
	}
	render_data.vertex_array->SetIndexBuffer(IndexBuffer::Create(indices.data(), kMaxIndices));

	const Ref<Texture> white = Texture2D::Create(1, 1);
	u32 white_pixel = 0xffffffff;
	white->SetData(&white_pixel, sizeof(u32));
	render_data.texture_slots[0] = white;

	std::array<int, kMaxTextureSlots> samplers{};
	std::iota(samplers.begin(), samplers.end(), 0);
	render_data.shader = Shader::Create(builtin_shaders::Source(builtin_shaders::Id::kRenderer2D));
	render_data.shader->Bind();
	render_data.shader->SetUniform1iv("uTextures", samplers.data(), kMaxTextureSlots);
}

void R2D::BeginScene(const CameraView& view) {
	render_data.view = view;
	render_data.vertices.clear();
	render_data.texture_count = 1;
}

void R2D::BeginScene(const glm::vec2 target_size) { BeginScene(Camera2D().View(target_size)); }

void R2D::EndScene() { Flush(); }

void R2D::DrawQuad(const glm::vec2 position, const glm::vec2 size, const glm::vec4& color) {
	PushQuad(position, size, color, 0.0f, {0.0f, 0.0f}, {1.0f, 1.0f});
}

void R2D::DrawQuad(const glm::vec2 position, const glm::vec2 size, const Ref<Texture>& texture) {
	DrawQuad(position, size, texture, {0.0f, 0.0f, 1.0f, 1.0f});
}

void R2D::DrawQuad(const glm::vec2 position, const glm::vec2 size, const Ref<Texture>& texture,
				   const glm::vec4& uv_rect, const glm::vec4& tint) {
	// Before picking a slot: a flush for room would clear the slot this quad was given.
	if (render_data.vertices.size() == kMaxVertices)
		Flush();

	const float slot = TextureSlot(texture);
	PushQuad(position, size, tint, slot, {uv_rect.x, uv_rect.y}, {uv_rect.x + uv_rect.z, uv_rect.y + uv_rect.w});
}

void R2D::DrawText(const std::string_view text, const assets::AssetHandle font_handle, const glm::vec2 position,
				   const float font_size, const glm::vec4& color) {
	const assets::FontAsset* font = ResolveFont(font_handle);
	const glm::vec2 atlas_size(font->texture->GetWidth(), font->texture->GetHeight());
	glm::vec2 pen = position;
	const float scale = font_size / font->font_size;

	for (const char c: text) {
		if (c == '\n') {
			pen = {position.x, pen.y - font->font_size * scale};
			continue;
		}

		const auto it = font->characters.find(c);
		if (it == font->characters.end())
			continue;

		const assets::FontAsset::Character& glyph = it->second;
		const glm::vec2 size = glm::vec2(glyph.location.z, glyph.location.w) * scale;
		const glm::vec2 origin = pen + glm::vec2(glyph.bearing.x, -glyph.bearing.y) * scale;
		const glm::vec4 uv_rect = glm::vec4(glyph.location) / glm::vec4(atlas_size, atlas_size);

		DrawQuad(origin, size, font->texture, uv_rect, color);

		// FreeType advances are in 1/64 pixel.
		pen.x += static_cast<float>(glyph.advance >> 6) * scale;
	}
}

float R2D::TextureSlot(const Ref<Texture>& texture) {
	for (u32 i = 1; i < render_data.texture_count; ++i) {
		if (*render_data.texture_slots[i] == *texture)
			return static_cast<float>(i);
	}

	if (render_data.texture_count == kMaxTextureSlots)
		Flush();

	render_data.texture_slots[render_data.texture_count] = texture;
	return static_cast<float>(render_data.texture_count++);
}

void R2D::PushQuad(const glm::vec2 position, const glm::vec2 size, const glm::vec4& color, const float texture_slot,
				   const glm::vec2 uv_min, const glm::vec2 uv_max) {
	if (render_data.vertices.size() == kMaxVertices)
		Flush();

	const glm::vec2 max = position + size;
	render_data.vertices.push_back({position, color, uv_min, texture_slot});
	render_data.vertices.push_back({{max.x, position.y}, color, {uv_max.x, uv_min.y}, texture_slot});
	render_data.vertices.push_back({max, color, uv_max, texture_slot});
	render_data.vertices.push_back({{position.x, max.y}, color, {uv_min.x, uv_max.y}, texture_slot});
}

void R2D::Flush() {
	BR_PROFILE_FUNCTION();

	Command::DisableDepth();
	if (!render_data.vertices.empty()) {
		render_data.shader->Bind();
		render_data.shader->SetUniformMat4("uVPmatrix", render_data.view.ViewProjection());
		for (u32 i = 0; i < render_data.texture_count; ++i)
			render_data.texture_slots[i]->Bind(i);

		render_data.vertex_buffer->SetBufferData(render_data.vertices.data(),
												 render_data.vertices.size() * sizeof(QuadVertex));
		render_data.vertex_array->Bind();
		Command::DrawIndexed(render_data.vertex_array, static_cast<u32>(render_data.vertices.size() / 4 * 6));
	}
	Command::EnableDepth();

	render_data.vertices.clear();
	render_data.texture_count = 1;
}
} // namespace bron
