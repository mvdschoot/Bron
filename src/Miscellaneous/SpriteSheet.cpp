#include "SpriteSheet.h"

namespace Steve {
	SpriteSheet::SpriteSheet(SpriteSheetProps& props)
		: _props(props)
	{

	}

	glm::vec2 SpriteSheet::getTexCoords(std::string name)
	{
		int num = -1;
		for(std::vector<SpriteID>::iterator x = _props.Sprites.begin();
			x != _props.Sprites.end();
			++x)
		{
			if (x->name == name){
				num = 0;
			}
		}
		CORE_ASSERT(num != -1, "Cannot find name {} in spritesheet", name);
		int width = _props.Texture->getWidth();
		int height = _props.Texture->getHeight();
		int row = num * _props.SpriteWidth / width;
		int column = num % _props.SpriteWidth;
		return {column * _props.SpriteWidth, row * _props.SpriteHeight};
	}
	
	glm::vec2 SpriteSheet::getTexCoords(uint32_t id)
	{
		int num = -1;
		for(std::vector<SpriteID>::iterator x = _props.Sprites.begin();
			x != _props.Sprites.end();
			++x)
		{
			if (x->id == id){
				num = 0;
			}
		}
		CORE_ASSERT(num != -1, "Cannot find name {} in spritesheet", name);
		int width = _props.Texture->getWidth();
		int height = _props.Texture->getHeight();
		int row = num * _props.SpriteWidth / width;
		int column = num % _props.SpriteWidth;
		return {column * _props.SpriteWidth, row * _props.SpriteHeight};
		
	}
}