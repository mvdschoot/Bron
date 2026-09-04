#pragma once

#include <memory>
#include <cstdarg>

#include "Bron/Core/Core.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace bron {

class BR_API Logger {
public:
	static void Init();
	static Ref<spdlog::logger>& GetCoreLogger() { return core_logger_; }
	static Ref<spdlog::logger>& GetAppLogger() { return app_logger_; }

private:
	static Ref<spdlog::logger> core_logger_;
	static Ref<spdlog::logger> app_logger_;
};

}  // namespace bron


#ifdef BR_DEBUG
	#define BR_CORE_ERROR(...) ::bron::Logger::GetCoreLogger()->error(__VA_ARGS__)
	#define BR_CORE_WARN(...) ::bron::Logger::GetCoreLogger()->warn(__VA_ARGS__)
	#define BR_CORE_INFO(...) ::bron::Logger::GetCoreLogger()->info(__VA_ARGS__)
	#define BR_CORE_TRACE(...) ::bron::Logger::GetCoreLogger()->trace(__VA_ARGS__)

	#define BR_APP_ERROR(...) ::bron::Logger::GetAppLogger()->error(__VA_ARGS__)
	#define BR_APP_WARN(...) ::bron::Logger::GetAppLogger()->warn(__VA_ARGS__)
	#define BR_APP_INFO(...) ::bron::Logger::GetAppLogger()->info(__VA_ARGS__)
	#define BR_APP_TRACE(...) ::bron::Logger::GetAppLogger()->trace(__VA_ARGS__)

	// Assertions. The do/while(0) wrapper is what makes these safe to use as the
	// body of an if without braces; the previous bare-brace form silently broke
	// any `if (c) ASSERT(...); else ...`.
	#define BR_APP_ASSERT(x, ...)                    \
		do {                                         \
			if (!(x)) {                              \
				BR_APP_ERROR(__VA_ARGS__);           \
				BR_DEBUGBREAK                        \
			}                                        \
		} while (0)
	#define BR_CORE_ASSERT(x, ...)                   \
		do {                                         \
			if (!(x)) {                              \
				BR_CORE_ERROR(__VA_ARGS__);          \
				BR_DEBUGBREAK                        \
			}                                        \
		} while (0)
#else
	#define BR_CORE_ERROR(...)
	#define BR_CORE_WARN(...)
	#define BR_CORE_INFO(...)
	#define BR_CORE_TRACE(...)

	#define BR_APP_ERROR(...)
	#define BR_APP_WARN(...)
	#define BR_APP_INFO(...)
	#define BR_APP_TRACE(...)

	// Release keeps the condition in an unevaluated context so it still has to
	// compile -- otherwise a Debug-only typo only surfaces in the Release CI leg.
	#define BR_APP_ASSERT(x, ...)      \
		do {                           \
			(void) sizeof(!(x));       \
		} while (0)
	#define BR_CORE_ASSERT(x, ...)     \
		do {                           \
			(void) sizeof(!(x));       \
		} while (0)
#endif

#define BR_APP_STATIC_ASSERT(x) static_assert(x)
#define BR_CORE_STATIC_ASSERT(x) static_assert(x)
