#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

namespace bron {
class Platform {
public:
	enum class API { kNone = 0, kOpenGl = 1 };

	enum class OS { kNone = 0, kWindows = 1, kLinux = 2, kMacOs = 3 };

public:
	static OS GetOs() {
#if defined(BR_PLATFORM_WINDOWS)
		return OS::kWindows;
#elif defined(BR_PLATFORM_LINUX)
		return OS::kLinux;
#elif defined(BR_PLATFORM_MACOS)
		return OS::kMacOs;
#else
		BR_CORE_ERROR("No Platform detected");
		return OS::kNone;
#endif
	}

	static API GetApi() {
#if defined(BR_RENDERER_OPENGL)
		return API::kOpenGl;
#else
		BR_CORE_ERROR("No rendering API selected");
		return API::kNone;
#endif
	}
};
} // namespace bron
