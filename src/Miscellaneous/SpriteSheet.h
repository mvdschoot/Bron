#ifndef __SPRITESHEET_H__
#define __SPRITESHEET_H__

#include "Cheets/Core/Core.h"
#include "Cheets/Core/Profiling.h"
#include "Cheets/Core/Logger.h"

#include "Cheets/Renderer/Texture.h"

#include "glm/glm.hpp"

#include <string>
#include <vector>

namespace Cheets {
	struct SpriteID
	{
		std::string name;
		uint32_t id;
	}

	struct SpriteSheetProps{
		Ref<Texture2D> Texture;
		uint32_t SpriteWidth;
		uint32_t SpriteHeight;
		std::vector<SpriteID> Sprites;
		uint32_t padding;
	};

	class SpriteSheet
	{
		public:
			SpriteSheet(SpriteSheetProps& props);

			const glm::vec2 getTexCoords(std::string name) const;
			const glm::vec2 getTexCoords(uint32_t id)const;

			uint32_t getSpriteWidth() const
			{
				return _props.SpriteWidth;
			}
			uint32_t getSpriteHeight() const
			{
				return _props.SpriteHeight;
			}
			Ref<Texture2D> getTexture() const
			{
				return _props.Texture;
			}
		private:
			SpriteSheetProps& _props;
	}
}


#endif // __SPRITESHEET_H__