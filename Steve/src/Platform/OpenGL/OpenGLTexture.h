#ifndef __OPENGLTEXTURE_H__
#define __OPENGLTEXTURE_H__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Steve/Graphics/Texture.h"

#include "stb_image.h"

#include <string>

namespace Steve
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(u32 width, u32 height);
		OpenGLTexture2D(const std::string& path);

		virtual ~OpenGLTexture2D() override;
		void setData(void* data, u32 size) override;
		void bind(u32 slot) override;

		static GLenum toGLEnum(u32 slot);

	protected:
		const std::string _path;
		u32 _channels;
		GLenum _internalFormat, _dataFormat;
	};
}

#endif // __OPENGLTEXTURE_H__
