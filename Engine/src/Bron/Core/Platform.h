#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

namespace Bron
{
	class Platform
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1
		};

		enum class OS
		{
			None = 0,
			Windows = 1,
			Linux = 2,
			MacOS = 3
		};

	public:
		static OS getOS()
		{
#if defined(BR_PLATFORM_WINDOWS)
			return OS::Windows;
#elif defined(BR_PLATFORM_LINUX)
			return OS::Linux;
#elif defined(BR_PLATFORM_MACOS)
			return OS::MacOS;
#else
			CORE_ERROR("No Platform detected");
			return OS::None;
#endif
		}

		static API getAPI()
		{
#if defined(BR_RENDERER_OPENGL)
			return API::OpenGL;
#else
			CORE_ERROR("No rendering API selected");
			return API::None;
#endif
		}
	};
}
