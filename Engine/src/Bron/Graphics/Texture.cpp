#include "Texture.h"

#include "Bron/Core/Platform.h"
#include "Platform/OpenGL/OpenGLTextTexture.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace bron
{
	Ref<Texture> Texture2D::Create(u32 width, u32 height)
	{
		switch (Platform::GetApi())
		{
		case Platform::API::kNone: BR_CORE_ASSERT(false, "No rendering API detected!");
		case Platform::API::kOpenGl: return CreateRef<OpenGLTexture2D>(width, height);
		}

		return nullptr;
	}

	Ref<Texture> Texture2D::Create(const char* path)
	{
		switch (Platform::GetApi())
		{
		case Platform::API::kNone: BR_CORE_ASSERT(false, "No rendering API detected!");
		case Platform::API::kOpenGl: return CreateRef<OpenGLTexture2D>(path);
		}

		return nullptr;
	}

	Ref<Texture> Texture2D::CreateFromMemory(const void* data, u32 size)
	{
		switch (Platform::GetApi())
		{
		case Platform::API::kNone: BR_CORE_ASSERT(false, "No rendering API detected!");
		case Platform::API::kOpenGl: return CreateRef<OpenGLTexture2D>(data, size);
		}

		return nullptr;
	}

	Ref<TextTexture> TextTexture::Create(u32 width, u32 height)
	{
		switch (Platform::GetApi())
		{
		case Platform::API::kNone: BR_CORE_ASSERT(false, "No rendering API detected!");
		case Platform::API::kOpenGl: return CreateRef<OpenGLTextTexture>(width, height);
		}

		return nullptr;
	}
}
