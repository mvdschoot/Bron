#include "OpenGLTexture.h"

namespace Steve
{
	OpenGLTexture2D::OpenGLTexture2D(u32 width, u32 height)
		: Texture2D(width, height), _channels(4), _internalFormat(GL_RGBA8), _dataFormat(GL_RGBA)
	{
		CH_PROFILE_FUNCTION();
		glCreateTextures(GL_TEXTURE_2D, 1, &p_rendererId);
		glTextureStorage2D(p_rendererId, 1, _internalFormat, width, height);

		glTextureParameteri(p_rendererId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(p_rendererId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(p_rendererId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(p_rendererId, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: _path(path)
	{
		CH_PROFILE_FUNCTION();
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;

		int width, height, channels;
		data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		p_width = width;
		p_height = height;
		_channels = channels;

		CORE_ASSERT(data, "Cannot load image data")

		if (channels == 3)
		{
			_internalFormat = GL_RGB8;
			_dataFormat = GL_RGB;
		}
		else if (channels == 4)
		{
			_internalFormat = GL_RGBA8;
			_dataFormat = GL_RGBA;
		}
		else
		{
			CORE_ASSERT(false, "Invalid number of channels");
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &p_rendererId);
		glTextureStorage2D(p_rendererId, 1, _internalFormat, p_width, p_height);

		glTextureParameteri(p_rendererId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(p_rendererId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(p_rendererId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(p_rendererId, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(p_rendererId, 0, 0, 0, width, height, _dataFormat, GL_UNSIGNED_BYTE, data);


		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		CH_PROFILE_FUNCTION();
		glDeleteTextures(1, &p_rendererId);
	}

	void OpenGLTexture2D::setData(void* data, u32 size)
	{
		CH_PROFILE_FUNCTION();

		CORE_ASSERT(size == p_width * p_height * _channels, "Data must be entire texture!");
		glTextureSubImage2D(p_rendererId, 0, 0, 0, p_width, p_height, _dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::bind(u32 slot)
	{
		CH_PROFILE_FUNCTION();
		glBindTextureUnit(slot, p_rendererId);
	}

	GLenum OpenGLTexture2D::toGLEnum(u32 slot)
	{
		return GL_TEXTURE0 + slot;
	}
}
