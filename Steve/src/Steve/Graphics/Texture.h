#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"

#include <string>

namespace Steve
{

	class STEVE_API Texture
	{
	public:
		virtual u32 getWidth() = 0;
		virtual u32 getHeight() = 0;

		virtual void setData(void* data, u32 size) = 0;

		virtual void bind(u32 slot) = 0;

		virtual u32 getID() = 0;
		virtual bool operator==(const Texture& other) const = 0;
	};

	class STEVE_API Texture2D : public Texture
	{
	public:
		static Ref<Texture> Create(u32 width, u32 height);
		static Ref<Texture> Create(const char* path);

	public:
		virtual ~Texture2D() {}
		u32 getWidth() override { return p_width; }
		u32 getHeight() override { return p_height; }

		u32 getID() override { return p_rendererId; }
		bool operator==(const Texture& other) const override { return p_rendererId == ((Texture2D&)other).p_rendererId; }
	protected:
		Texture2D() : p_rendererId(-1), p_width(-1), p_height(-1) {}
		Texture2D(u32 width, u32 height) : p_rendererId(-1), p_width(width), p_height(height) {}
		u32 p_rendererId;
		u32 p_width, p_height;
	};

	class STEVE_API TextTexture : public Texture2D
	{
	public:
		TextTexture() : Texture2D() {}
		TextTexture(u32 width, u32 height) : Texture2D(width, height) {}
		virtual ~TextTexture() override {}
		static Ref<TextTexture> Create(u32 width, u32 height);

		virtual void setLetter(void* data, u32 x, u32 y, u32 width, u32 height) = 0;
	};

} // namespace Steve


#endif // __TEXTURE_H__
