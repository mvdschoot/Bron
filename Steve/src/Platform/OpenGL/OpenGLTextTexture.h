#ifndef __OPENGLTEXTTEXTURE_HEADER__
#define __OPENGLTEXTTEXTURE_HEADER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Steve/Graphics/Texture.h"

#include "OpenGLTexture.h"

#include "stb_image.h"

#include <string>


namespace Steve
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


#endif