#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Profiling.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Bron/Graphics/Texture.h"

#include "stb_image.h"

#include <string>

namespace bron {
class OpenGLTexture2D : public Texture2D {
public:
	OpenGLTexture2D(u32 width, u32 height);
	OpenGLTexture2D(const std::string& path);
	// Decodes a compressed image (png/jpg/...) that lives in memory, e.g. a texture
	// embedded inside a .glb/.fbx container.
	OpenGLTexture2D(const void* buffer, u32 size);

	virtual ~OpenGLTexture2D() override;
	void SetData(void* data, u32 size) override;
	void Bind(u32 slot) override;

	static GLenum ToGlEnum(u32 slot);

private:
	// Shared upload path for both the file and the in-memory constructor.
	void UploadPixels(const void* pixels, int width, int height, int channels);

protected:
	const std::string path_;
	u32 channels_;
	GLenum internal_format_, data_format_;
};
} // namespace bron
