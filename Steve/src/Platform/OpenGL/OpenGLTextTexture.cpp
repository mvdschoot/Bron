#include "OpenGLTextTexture.h"

namespace Steve
{
	OpenGLTextTexture::OpenGLTextTexture(u32 width, u32 height) :
		TextTexture(width, height), _internalFormat(GL_RED), _dataFormat(GL_RED)
	{
		CH_PROFILE_FUNCTION();

		glGenTextures(1, &p_rendererId);

		glBindTexture(GL_TEXTURE_2D, p_rendererId);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, p_width, p_height, 0, _dataFormat, GL_UNSIGNED_BYTE, nullptr);

		glTextureParameteri(p_rendererId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(p_rendererId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(p_rendererId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(p_rendererId, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	void OpenGLTextTexture::setData(void* data, u32 size)
	{
		const u32 bpp = _dataFormat == GL_RGBA ? 4 : 3;
		CORE_ASSERT(size == p_width * p_height * bpp, "Data must be entire texture!");

		glTextureSubImage2D(p_rendererId, 0, 0, 0, p_width, p_height, _dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTextTexture::bind(u32 slot)
	{
		CH_PROFILE_FUNCTION();
		glBindTextureUnit(slot, p_rendererId);
	}


	void OpenGLTextTexture::setLetter(void* data, u32 x, u32 y, u32 width, u32 height)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, _internalFormat, GL_UNSIGNED_BYTE, data);
	}
}