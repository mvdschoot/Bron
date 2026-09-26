//
// Created by mathi on 26-9-2026.
//

#include "FontLoader.h"

#include "Bron/Core/Logger.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include "Bron/Graphics/Texture.h"

namespace bron {

static FT_Library ft_library;

std::optional<ImportedFont> FontLoader::Import(const std::filesystem::path& path, float import_font_size) {
	if (!ft_library) {
		FT_Init_FreeType(&ft_library);
	}

	ImportedFont font;
	FT_Face ft_font;

	if (FT_New_Face(ft_library, path.string().c_str(), 0, &ft_font)) {
		BR_CORE_ERROR("Failed to load font from location {}", path.string());
		return std::nullopt;
	}

	FT_Set_Pixel_Sizes(ft_font, 0, import_font_size);
	FT_Select_Charmap(ft_font, ft_encoding_unicode);

	font.texture = TextTexture::Create(import_font_size * (127 - 32), import_font_size);

	// Flip character bitmap
	FT_Matrix matrix = {1, 0, 0, -1};
	matrix.xx = 0x10000L;
	matrix.xy = 0;
	matrix.yx = 0;
	matrix.yy = -0x10000L;

	FT_Vector vector = {0, 0};
	FT_Set_Transform(ft_font, &matrix, &vector);

	u32 x = 0;
	const FT_GlyphSlot g = ft_font->glyph;
	for (unsigned char c = 32; c < 127; c++) {
		// load character glyph
		const FT_UInt glyph = FT_Get_Char_Index(ft_font, c);
		BR_CORE_ASSERT(glyph != 0, "Character not present in font file {}", path.string());

		if (FT_Load_Glyph(ft_font, glyph, FT_LOAD_RENDER)) {
			BR_CORE_ERROR("Failed to load character from font {}.", path.string());
			continue;
		}

		ImportedCharacter character = {glm::ivec4(x, 0, g->bitmap.width, g->bitmap.rows),
									   glm::ivec2(g->bitmap_left, g->bitmap_top), static_cast<u32>(g->advance.x)};
		font.characters.emplace(c, character);

		font.texture->SetLetter(g->bitmap.buffer, character.location.x, character.location.y, character.location.z,
								character.location.w);
		x += g->bitmap.width;
	}
	font.font_size = import_font_size;
	FT_Done_Face(ft_font);
	return font;
}
} // namespace bron
