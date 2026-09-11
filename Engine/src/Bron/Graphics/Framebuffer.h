#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

#include <vector>

namespace bron {
enum class FramebufferTextureFormat {kNone, kRgba8, kRedInteger, kDepth24Stencil8};

struct FramebufferSpecification {
	u32 width;
	u32 height;

	std::vector<FramebufferTextureFormat> attachments;

	bool swap_chain_target = false;
};

class Framebuffer {
public:
	Framebuffer() {};

	virtual ~Framebuffer() {};
	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	virtual void Invalidate() = 0;
	virtual u32 GetColorAttachId(u32 index) = 0;
	virtual u32 GetDepthStencilAttachId() = 0;
	/// Reads one pixel out of an integer colour attachment. Coordinates are in framebuffer
	/// space: origin bottom-left, so callers working from a top-left origin must flip Y.
	virtual int ReadPixelInt(u32 index, int position_x, int position_y) = 0;
	virtual void ClearAttachmentInt(u32 index, int value) = 0;


	static Ref<Framebuffer> Create(FramebufferSpecification& spec);
};
} // namespace bron
