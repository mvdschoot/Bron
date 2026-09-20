#include "Platform.h"

#ifdef BR_PLATFORM_WINDOWS
	#include <windows.h>
#endif

namespace bron {
std::filesystem::path ExecutableDirectory() {
#ifdef BR_PLATFORM_WINDOWS
	wchar_t buffer[MAX_PATH];

	const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
	if (length == 0 || length == MAX_PATH) {
		BR_CORE_ERROR("Could not determine the executable's location; falling back to the working directory.");
		return std::filesystem::current_path();
	}

	return std::filesystem::path(buffer, buffer + length).parent_path();
#else
	std::error_code error;
	const std::filesystem::path self = std::filesystem::read_symlink("/proc/self/exe", error);
	if (error) {
		BR_CORE_ERROR("Could not determine the executable's location: {}", error.message());
		return std::filesystem::current_path();
	}

	return self.parent_path();
#endif
}
} // namespace bron
