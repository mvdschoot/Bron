#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

#include <string>

namespace bron {

class BR_API Texture {
public:
	virtual u32 GetWidth() = 0;
	virtual u32 GetHeight() = 0;

	virtual void SetData(void* data, u32 size) = 0;

	virtual void Bind(u32 slot) = 0;

	virtual u32 GetId() = 0;
	virtual bool operator==(const Texture& other) const = 0;
};

class BR_API Texture2D : public Texture {
public:
	static Ref<Texture> Create(u32 width, u32 height);
	static Ref<Texture> Create(const char* path);
	// Creates a texture from a compressed image (png/jpg/...) held in memory, such as a
	// texture embedded inside a model file (.glb, binary .fbx).
	static Ref<Texture> CreateFromMemory(const void* data, u32 size);

public:
	virtual ~Texture2D() {}
	u32 GetWidth() override { return width_; }
	u32 GetHeight() override { return height_; }

	u32 GetId() override { return renderer_id_; }
	bool operator==(const Texture& other) const override { return renderer_id_ == ((Texture2D&) other).renderer_id_; }

protected:
	Texture2D() : renderer_id_(-1), width_(-1), height_(-1) {}
	Texture2D(u32 width, u32 height) : renderer_id_(-1), width_(width), height_(height) {}
	u32 renderer_id_;
	u32 width_, height_;
};

class BR_API TextTexture : public Texture2D {
public:
	TextTexture() : Texture2D() {}
	TextTexture(u32 width, u32 height) : Texture2D(width, height) {}
	virtual ~TextTexture() override {}
	static Ref<TextTexture> Create(u32 width, u32 height);

	virtual void SetLetter(void* data, u32 x, u32 y, u32 width, u32 height) = 0;
};

} // namespace bron
