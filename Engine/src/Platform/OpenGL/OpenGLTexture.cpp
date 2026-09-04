#include "OpenGLTexture.h"

namespace bron
{
	OpenGLTexture2D::OpenGLTexture2D(u32 width, u32 height)
		: Texture2D(width, height), channels_(4), internal_format_(GL_RGBA8), data_format_(GL_RGBA)
	{
		BR_PROFILE_FUNCTION();
		glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id_);
		glTextureStorage2D(renderer_id_, 1, internal_format_, width, height);

		glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: path_(path)
	{
		BR_PROFILE_FUNCTION();
		stbi_set_flip_vertically_on_load(1);

		int width, height, channels;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		BR_CORE_ASSERT(data, "Cannot load image data from file: {}", path);

		UploadPixels(data, width, height, channels);

		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(const void* buffer, u32 size)
	{
		BR_PROFILE_FUNCTION();
		stbi_set_flip_vertically_on_load(1);

		int width, height, channels;
		stbi_uc* data = stbi_load_from_memory(static_cast<const stbi_uc*>(buffer), static_cast<int>(size), &width,
											  &height, &channels, 0);

		BR_CORE_ASSERT(data, "Cannot load embedded image data ({} bytes)", size);

		UploadPixels(data, width, height, channels);

		stbi_image_free(data);
	}

	void OpenGLTexture2D::UploadPixels(const void* pixels, int width, int height, int channels)
	{
		BR_PROFILE_FUNCTION();

		width_ = width;
		height_ = height;
		channels_ = channels;

		if (channels == 3)
		{
			internal_format_ = GL_RGB8;
			data_format_ = GL_RGB;
		}
		else if (channels == 4)
		{
			internal_format_ = GL_RGBA8;
			data_format_ = GL_RGBA;
		}
		else
		{
			BR_CORE_ASSERT(false, "Invalid number of channels ({})", channels);
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id_);
		glTextureStorage2D(renderer_id_, 1, internal_format_, width_, height_);

		glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Rows of an RGB image are not 4-byte aligned, which is what GL expects by default.
		glPixelStorei(GL_UNPACK_ALIGNMENT, channels == 4 ? 4 : 1);
		glTextureSubImage2D(renderer_id_, 0, 0, 0, width_, height_, data_format_, GL_UNSIGNED_BYTE, pixels);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		BR_PROFILE_FUNCTION();
		glDeleteTextures(1, &renderer_id_);
	}

	void OpenGLTexture2D::SetData(void* data, u32 size)
	{
		BR_PROFILE_FUNCTION();

		BR_CORE_ASSERT(size == width_ * height_ * channels_, "Data must be entire texture!");
		glTextureSubImage2D(renderer_id_, 0, 0, 0, width_, height_, data_format_, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(u32 slot)
	{
		BR_PROFILE_FUNCTION();
		glBindTextureUnit(slot, renderer_id_);
	}

	GLenum OpenGLTexture2D::ToGlEnum(u32 slot)
	{
		return GL_TEXTURE0 + slot;
	}
}
