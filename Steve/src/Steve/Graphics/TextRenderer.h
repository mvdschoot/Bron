#ifndef _TEXT_RENDERER__
#define _TEXT_RENDERER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "Texture.h"

#include "glm/glm.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include "freetype/ftglyph.h"


#include <string>
#include <map>

#include "Steve/Graphics/Renderer/2D.h"


namespace Steve
{
	struct Character
	{
		glm::ivec4 Location;
		glm::ivec2 Bearing;
		unsigned int Advance;
	};

	struct Font
	{
		FT_Face FT_font;
		std::map<char, Character> Characters;
		Ref<TextTexture> Texture;
	};

	class TextRenderer
	{
	public:
		static void Init();
		static Font& LoadUnicodeFont(const char* font_location, unsigned int init_font_height);

		static void RenderText(const char* text, float x, float y, float scale, glm::vec4 color);
		static void RenderText(const char* text, Font& font, float x, float y, float scale, glm::vec4 color);
	};
}

#endif
