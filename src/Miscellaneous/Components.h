#ifndef __COMPONENTS_HEADER
#define __COMPONENTS_HEADER

#include "Steve.h"

namespace Steve
{
	struct TransformComponent : public Component
	{
		glm::vec3 Transform;

		TransformComponent() = default;
		TransformComponent(const glm::vec3 trans) : Transform(trans) {}

		operator const glm::vec3() const {return Transform;}
	};

	struct LineComponent : public Component
	{
		glm::vec3 A;
		glm::vec3 B;

		LineComponent() = default;
		LineComponent(glm::vec3 a, glm::vec3 b) : A(a), B(b) {}
	};

	struct SpriteComponent : public Component
	{
		Ref<Texture> TextureRef;

		SpriteComponent() = default;
		SpriteComponent(Ref<Texture> texture) : TextureRef(std::move(texture)) {}
		SpriteComponent(const char* loc)
		{
			TextureRef = Texture2D::Create(loc);
		}

		operator const glm::uint32_t() const {return TextureRef->getID();}
		operator const Ref<Texture>() const { return TextureRef; }
	};

	struct ColorComponent : public Component
	{
		glm::vec4 Color;

		ColorComponent() = default;
		ColorComponent(float r, float g, float b, float a) : Color({r, g, b, a}) {}
		ColorComponent(const glm::vec4 color) : Color(color) {}

		operator const glm::vec4() const { return Color; }
	};

	struct SizeComponent : public Component
	{
		glm::vec3 Size;

		SizeComponent() = default;
		SizeComponent(float x, float y, float z) : Size({ x, y, z }) {}
		SizeComponent(const glm::vec4 size) : Size(size) {}

		operator const glm::vec3() const { return Size; }
	};
}

#endif