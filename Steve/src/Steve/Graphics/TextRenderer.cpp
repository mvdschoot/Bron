#include "TextRenderer.h"

#include <iostream>

namespace Steve
{
	struct TextRenderData
	{
		FT_Library FT_library;
		std::vector<Font> Fonts;
		u32 ShaderId;
	};
	static TextRenderData s_text_data;

	void TextRenderer::Init()
	{
		if (FT_Init_FreeType(&s_text_data.FT_library)) CORE_ERROR("Could not initialise FreeText library");

		s_text_data.ShaderId = R2D::AddShader(
			Shader::CreateShaderFromLocation("../Steve/Assets/text_shader.glsl"), 
			BufferLayout({
				{"a_Position", ShaderDataType::Float2},
				{"a_Color", ShaderDataType::Float4},
				{"a_TexCoord", ShaderDataType::Float2},
				{"a_TexIndex", ShaderDataType::Float}
			})
		);
	}

	Font& TextRenderer::LoadUnicodeFont(const char* font_location, unsigned int init_font_height)
	{
		s_text_data.Fonts.emplace_back();
		Font& font = s_text_data.Fonts.back();

		if (FT_New_Face(s_text_data.FT_library, font_location, 0, &font.FT_font))	CORE_ERROR("Failed to load font");
		FT_Set_Pixel_Sizes(font.FT_font, 0, init_font_height);

		FT_Select_Charmap(font.FT_font, ft_encoding_unicode);

		font.Texture = TextTexture::Create(init_font_height * (129 - 32), init_font_height);

		// Flip character bitmap
		FT_Matrix matrix = {1, 0, 0, -1};
		matrix.xx = 0x10000L;
		matrix.xy = 0;
		matrix.yx = 0;
		matrix.yy = -0x10000L;

		FT_Vector vector = { 0, 0 };
		FT_Set_Transform(font.FT_font, &matrix, &vector);

		u32 x = 0;
		const FT_GlyphSlot g = font.FT_font->glyph;
        for (unsigned char c = 32; c < 127; c++)
        {
            // load character glyph
			FT_UInt glyph = FT_Get_Char_Index(font.FT_font, c);
			CORE_ASSERT(glyph != 0, "Character not present in font file")

            if (FT_Load_Glyph(font.FT_font, glyph, FT_LOAD_RENDER))
            {
				CORE_ERROR("failed to load character.");
                continue;
            }

        	Character character = {
                glm::ivec4(x, 0, g->bitmap.width, g->bitmap.rows),
                glm::ivec2(g->bitmap_left, g->bitmap_top),
				g->advance.x
            };
            font.Characters.emplace(c, character);

			font.Texture->setLetter(g->bitmap.buffer, character.Location.x, character.Location.y, character.Location.z, character.Location.w);
			x += g->bitmap.width;

        }
		FT_Done_Face(font.FT_font);
		return font;
	}

	void TextRenderer::RenderText(const char* text, float x, float y, float scale, glm::vec4 color)
	{
		CORE_ASSERT(!s_text_data.Fonts.empty(), "No font loaded!");
		RenderText(text, s_text_data.Fonts.front(), x, y, scale, color);
	}

	void TextRenderer::RenderText(const char* text, Font& font, float x, float y, float scale, glm::vec4 color)
	{
		u32 old_state = R2D::GetActiveShader();
		R2D::ActiveShader(s_text_data.ShaderId);

        for (const char* c = text; *c != 0; c++)
        {
            Character ch = font.Characters[*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - ch.Bearing.y * scale;

            float w = ch.Location.z * scale;
            float h = ch.Location.w * scale;

			glm::vec4 txtLoc = {
				((float)ch.Location.x) / font.Texture->getWidth(),
				((float)ch.Location.y) / font.Texture->getHeight(),
				(float)ch.Location.z / font.Texture->getWidth(),
				(float)ch.Location.w / font.Texture->getHeight()
			};
			R2D::DrawQuad({ xpos, ypos }, { w, h }, font.Texture, txtLoc);

            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }

		R2D::ActiveShader(old_state);
	}
}
