#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Profiling.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Bron/Graphics/Texture.h"

#include "OpenGLTexture.h"

#include "stb_image.h"

#include <string>


namespace Bron
{
	class OpenGLTextTexture : public TextTexture
	{
	public:
		OpenGLTextTexture(u32 width, u32 height);

		void setData(void* data, u32 size) override;
		void bind(u32 slot) override; 

		void setLetter(void* data, u32 x, u32 y, u32 width, u32 height) override;
	private:
		GLenum _internalFormat, _dataFormat;
	};
}
