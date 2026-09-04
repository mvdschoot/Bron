#include "OpenGLTextTexture.h"

namespace bron {
OpenGLTextTexture::OpenGLTextTexture(u32 width, u32 height) :
	TextTexture(width, height), internal_format_(GL_RED), data_format_(GL_RED) {
	BR_PROFILE_FUNCTION();

	glGenTextures(1, &renderer_id_);

	glBindTexture(GL_TEXTURE_2D, renderer_id_);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, internal_format_, width_, height_, 0, data_format_, GL_UNSIGNED_BYTE, nullptr);

	glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void OpenGLTextTexture::SetData(void* data, u32 size) {
	const u32 bpp = data_format_ == GL_RGBA ? 4 : 3;
	BR_CORE_ASSERT(size == width_ * height_ * bpp, "Data must be entire texture!");

	glTextureSubImage2D(renderer_id_, 0, 0, 0, width_, height_, data_format_, GL_UNSIGNED_BYTE, data);
}

void OpenGLTextTexture::Bind(u32 slot) {
	BR_PROFILE_FUNCTION();
	glBindTextureUnit(slot, renderer_id_);
}


void OpenGLTextTexture::SetLetter(void* data, u32 x, u32 y, u32 width, u32 height) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, internal_format_, GL_UNSIGNED_BYTE, data);
}
} // namespace bron
