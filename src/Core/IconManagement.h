#ifndef __ICONMNGNG_HEADRE__
#define __ICONMNGNG_HEADRE__
#include <map>

#include "Steve/Graphics/Texture.h"

namespace Steve
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

#endif