#include "OpenGLTexture.h"

namespace Bron
{
	OpenGLTexture2D::OpenGLTexture2D(u32 width, u32 height)
		: Texture2D(width, height), _channels(4), _internalFormat(GL_RGBA8), _dataFormat(GL_RGBA)
	{
		BR_PROFILE_FUNCTION();
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
		BR_PROFILE_FUNCTION();
		stbi_set_flip_vertically_on_load(1);

		int width, height, channels;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		CORE_ASSERT(data, "Cannot load image data from file: {}", path)

		uploadPixels(data, width, height, channels);

		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(const void* buffer, u32 size)
	{
		BR_PROFILE_FUNCTION();
		stbi_set_flip_vertically_on_load(1);

		int width, height, channels;
		stbi_uc* data = stbi_load_from_memory(static_cast<const stbi_uc*>(buffer), static_cast<int>(size), &width,
											  &height, &channels, 0);

		CORE_ASSERT(data, "Cannot load embedded image data ({} bytes)", size)

		uploadPixels(data, width, height, channels);

		stbi_image_free(data);
	}

	void OpenGLTexture2D::uploadPixels(const void* pixels, int width, int height, int channels)
	{
		BR_PROFILE_FUNCTION();

		p_width = width;
		p_height = height;
		_channels = channels;

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
			CORE_ASSERT(false, "Invalid number of channels ({})", channels);
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &p_rendererId);
		glTextureStorage2D(p_rendererId, 1, _internalFormat, p_width, p_height);

		glTextureParameteri(p_rendererId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(p_rendererId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(p_rendererId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(p_rendererId, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Rows of an RGB image are not 4-byte aligned, which is what GL expects by default.
		glPixelStorei(GL_UNPACK_ALIGNMENT, channels == 4 ? 4 : 1);
		glTextureSubImage2D(p_rendererId, 0, 0, 0, p_width, p_height, _dataFormat, GL_UNSIGNED_BYTE, pixels);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		BR_PROFILE_FUNCTION();
		glDeleteTextures(1, &p_rendererId);
	}

	void OpenGLTexture2D::setData(void* data, u32 size)
	{
		BR_PROFILE_FUNCTION();

		CORE_ASSERT(size == p_width * p_height * _channels, "Data must be entire texture!");
		glTextureSubImage2D(p_rendererId, 0, 0, 0, p_width, p_height, _dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::bind(u32 slot)
	{
		BR_PROFILE_FUNCTION();
		glBindTextureUnit(slot, p_rendererId);
	}

	GLenum OpenGLTexture2D::toGLEnum(u32 slot)
	{
		return GL_TEXTURE0 + slot;
	}
}
