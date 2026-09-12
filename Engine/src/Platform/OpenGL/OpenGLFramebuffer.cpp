#include "OpenGLFramebuffer.h"

#include "glad/glad.h"

namespace bron {
namespace utils {

static u32 AttachTexture(GLint attachment, GLint internal_format, GLenum format, GLenum type, GLint filter,
						 GLuint width, GLuint height) {
	u32 attachment_id;

	glCreateTextures(GL_TEXTURE_2D, 1, &attachment_id);
	glBindTexture(GL_TEXTURE_2D, attachment_id);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, attachment_id, 0);

	return attachment_id;
}

} // namespace utils

OpenGLFramebuffer::OpenGLFramebuffer(FramebufferSpecification& spec) : spec_(spec) { OpenGLFramebuffer::Invalidate(); }

OpenGLFramebuffer::~OpenGLFramebuffer() { Destroy(); }

// The attachments are owned by this framebuffer, so they go with it - deleting only the
// framebuffer object leaves the textures alive.
void OpenGLFramebuffer::Destroy() {
	glDeleteFramebuffers(1, &renderer_id_);
	glDeleteTextures(1, &depth_stencil_attachment_);
	glDeleteTextures(static_cast<GLsizei>(color_attachments_.size()), color_attachments_.data());

	renderer_id_ = 0;
	color_attachments_.clear();
	color_formats_.clear();
	depth_stencil_attachment_ = 0;
}

void OpenGLFramebuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);
	glViewport(0, 0, spec_.width, spec_.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLFramebuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void OpenGLFramebuffer::Invalidate() {
	// Invalidate() is a resize as much as a first-time build, so anything from a previous
	// call has to go first, otherwise every viewport resize leaks a framebuffer and its
	// textures.
	Destroy();

	glCreateFramebuffers(1, &renderer_id_);
	glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);

	// The colour attachment points have to be numbered densely from 0, independently of
	// where the formats sit in the spec - a depth format listed in the middle must not
	// leave a hole in the colour numbering.
	u32 color_index = 0;

	for (const FramebufferTextureFormat format: spec_.attachments) {
		switch (format) {
			case FramebufferTextureFormat::kRgba8:
				color_attachments_.push_back(utils::AttachTexture(GL_COLOR_ATTACHMENT0 + color_index, GL_RGBA8, GL_RGBA,
																  GL_UNSIGNED_BYTE, GL_LINEAR, spec_.width,
																  spec_.height));
				color_formats_.push_back(format);
				color_index++;
				break;
			case FramebufferTextureFormat::kRedInteger:
				// Integer textures cannot be filtered - GL_LINEAR makes them incomplete
				// the moment anything samples them.
				color_attachments_.push_back(utils::AttachTexture(GL_COLOR_ATTACHMENT0 + color_index, GL_R32I,
																  GL_RED_INTEGER, GL_INT, GL_NEAREST, spec_.width,
																  spec_.height));
				color_formats_.push_back(format);
				color_index++;
				break;
			case FramebufferTextureFormat::kDepth24Stencil8:
				depth_stencil_attachment_ =
						utils::AttachTexture(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL,
											 GL_UNSIGNED_INT_24_8, GL_LINEAR, spec_.width, spec_.height);
				break;
			default:
				BR_CORE_ASSERT(false, "Cannot attach a texture of this type to the framebuffer");
				break;
		}
	}

	// A framebuffer draws to GL_COLOR_ATTACHMENT0 alone unless told otherwise: every
	// attachment past the first stays GL_NONE, and the fragment outputs bound to them are
	// discarded. Without this the entity-id attachment is never written.
	if (color_attachments_.empty()) {
		glDrawBuffer(GL_NONE);
	} else {
		std::vector<GLenum> buffers(color_attachments_.size());
		for (u32 i = 0; i < buffers.size(); ++i)
			buffers[i] = GL_COLOR_ATTACHMENT0 + i;

		glDrawBuffers(static_cast<GLsizei>(buffers.size()), buffers.data());
	}

	BR_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete");
}

u32 OpenGLFramebuffer::GetColorAttachId(u32 index) { return color_attachments_[index]; }

u32 OpenGLFramebuffer::GetDepthStencilAttachId() { return depth_stencil_attachment_; }

int OpenGLFramebuffer::ReadPixelInt(u32 index, int position_x, int position_y) {
	BR_CORE_ASSERT(index < color_attachments_.size(), "Color attachment index out of range: {}", index);

	// Reads are driven from the panel, long after the scene pass unbound this framebuffer,
	// so it has to be made current again. Binding it as the *read* target only leaves the
	// draw target alone, and skips the glClear that Bind() does - which would wipe the
	// frame that is about to be presented.
	glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer_id_);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + index);

	int data = -1;
	glReadPixels(position_x, position_y, 1, 1, GL_RED_INTEGER, GL_INT, &data);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	return data;
}

void OpenGLFramebuffer::ClearAttachmentInt(u32 index, const int value) {
	BR_CORE_ASSERT(index < color_attachments_.size(), "Color attachment index out of range: {}", index);

	GLenum format;
	switch (color_formats_[index]) {
		case FramebufferTextureFormat::kRgba8:
			format = GL_RGBA;
			break;
		case FramebufferTextureFormat::kRedInteger:
			format = GL_RED_INTEGER;
			break;
		default:
			BR_CORE_ASSERT(false, "Cannot find the OpenGL format from the specified attachment; attachment_id: {}",
						   index);
			return;
	}

	glClearTexImage(color_attachments_[index], 0, format, GL_INT, &value);
}

} // namespace bron
