#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"

namespace Steve
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
			Linux = 2
		};

	public:
		static OS getOS()
		{
#if defined(CH_PLATFORM_WINDOWS)
			return OS::Windows;
#elif defined(CH_PLATFORM_LINUX)
					return OS::Linux;
#else
					CORE_ERROR("No Platform detected");
					return OS::None;
#endif
		}

		static API getAPI()
		{
#if defined(CH_API_OPENGL)
			return API::OpenGL;
#else
					CORE_ERROR("No Platform detected");
					return API::None;
#endif
		}
	};
}

#endif // __PLATFORM_H__
