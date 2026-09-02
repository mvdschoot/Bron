#pragma once
#include <map>

#include "Bron/Graphics/Texture.h"

namespace Bron::Editor
{
	class Icons
	{
	public:
		// struct Icon
		// {
		// 	std::string Name;
		// 	Ref<Texture> Texture;
		// };

		static void Load();
		static uint32_t GetIconId(const char* name);

	private:
		static std::map<std::string, Ref<Texture>> icons;
	};
}
