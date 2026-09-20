#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

#include <filesystem>

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

/// The directory the running executable sits in.
///
/// Never the working directory: that is whatever the shell, the launcher or the debugger
/// happened to be in, and a game launched from a shortcut has no relationship to it at
/// all. Anything shipped beside the binary - a manifest, the assets it names - is found
/// relative to this.
std::filesystem::path ExecutableDirectory();
} // namespace bron
