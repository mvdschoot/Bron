#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Profiling.h"

#include "Texture.h"

#include "glm/glm.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include "freetype/ftglyph.h"


#include <string>
#include <map>

#include "Bron/Graphics/Renderer/2D.h"


namespace bron {
struct Character {
	glm::ivec4 Location;
	glm::ivec2 Bearing;
	unsigned int Advance;
};

struct Font {
	FT_Face FT_font;
	std::map<char, Character> Characters;
	Ref<TextTexture> Texture;
};

class TextRenderer {
public:
	static void Init();
	static Font& LoadUnicodeFont(const char* font_location, unsigned int init_font_height);

	static void RenderText(const char* text, float x, float y, float scale, glm::vec4 color);
	static void RenderText(const char* text, Font& font, float x, float y, float scale, glm::vec4 color);
};
} // namespace bron
