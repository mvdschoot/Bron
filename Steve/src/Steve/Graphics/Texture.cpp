#include "Texture.h"

#include "Steve/Core/Platform.h"
#include "Platform/OpenGL/OpenGLTextTexture.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Steve
{
	Ref<Texture> Texture2D::Create(u32 width, u32 height)
	{
		switch (Platform::getAPI())
		{
		case Platform::API::None: CORE_ASSERT(false, "No rendering API detected!");
		case Platform::API::OpenGL: return createRef<OpenGLTexture2D>(width, height);
		}

		return nullptr;
	}

	Ref<Texture> Texture2D::Create(const char* path)
	{
		switch (Platform::getAPI())
		{
		case Platform::API::None: CORE_ASSERT(false, "No rendering API detected!");
		case Platform::API::OpenGL: return createRef<OpenGLTexture2D>(path);
		}

		return nullptr;
	}

	Ref<TextTexture> TextTexture::Create(u32 width, u32 height)
	{
		switch (Platform::getAPI())
		{
		case Platform::API::None: CORE_ASSERT(false, "No rendering API detected!");
		case Platform::API::OpenGL: return createRef<OpenGLTextTexture>(width, height);
		}

		return nullptr;
	}
}
