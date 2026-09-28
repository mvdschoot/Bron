//
// Created by mathi on 26-9-2026.
//


#pragma once
#include "Bron/Core/Core.h"
#include "glm/glm.hpp"


#include <filesystem>
#include <map>
#include <optional>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace bron {
class TextTexture;
struct ImportedCharacter {
	glm::ivec4 location;
	glm::ivec2 bearing;
	unsigned int advance;
};

struct ImportedFont {
	std::map<char, ImportedCharacter> characters;
	Ref<TextTexture> texture;
	float font_size;
};

constexpr float kImportFontSize = 50;

class FontLoader {
public:
	// Only imports valid ascii characters (number 32 to 127 (excl.))
	static std::optional<ImportedFont> Import(const std::filesystem::path& path,
											  float import_font_size = kImportFontSize);
	static std::optional<ImportedFont> Import(const u8* buffer, usize buffer_size,
											  float import_font_size = kImportFontSize);

private:
	static std::optional<ImportedFont> Import(FT_Face font, float import_font_size = kImportFontSize);
};

} // namespace bron
