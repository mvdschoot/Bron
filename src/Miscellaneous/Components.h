#ifndef __COMPONENTS_HEADER
#define __COMPONENTS_HEADER

#include "Cheets/Core/Core.h"
#include "Cheets/Core/Profiling.h"
#include "Cheets/Core/Logger.h"

#include "Cheets/Scene/Component.h"

#include "Cheets/Renderer/Texture.h"

#include "glm/glm.hpp"

namespace Cheets
{
	struct TransformComponent : public Component
	{
		glm::vec3 Transform;
		TransformComponent(glm::vec3 trans) : Component(id), Transform(trans) {}
		operator const glm::vec3(){return Transform;}
	};

	struct SpriteComponent : public Component
	{
		Ref<Texture> TextureRef;

		SpriteComponent(Ref<Texture> texture) : TextureRef(texture) {}
		SpriteComponent(std::string loc) {TextureRef = Texture2D::Create(loc);}
		operator const uint32_t(){return TextureRef->getID();}
	};
}

#endif