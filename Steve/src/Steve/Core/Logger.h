#ifndef LOG_HEADER
#define LOG_HEADER

#include <memory>
#include <cstdarg>

#include "Steve/Core/Core.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Steve
{
	class STEVE_API Logger
	{
	public:
		// Logger();
		// ~Logger();

		static void Init();
		static Ref<spdlog::logger>& getCoreLogger() { return _core_logger; }
		static Ref<spdlog::logger>& getAppLogger() { return _app_logger; }

	private:
		static Ref<spdlog::logger> _core_logger;
		static Ref<spdlog::logger> _app_logger;
	};
}


#ifdef CH_DEBUG
	#define CORE_ERROR(...)		::Steve::Logger::getCoreLogger()->error(__VA_ARGS__)
	#define CORE_WARN(...) 		::Steve::Logger::getCoreLogger()->warn(__VA_ARGS__)
	#define CORE_INFO(...) 		::Steve::Logger::getCoreLogger()->info(__VA_ARGS__)
	#define CORE_TRACE(...) 	::Steve::Logger::getCoreLogger()->trace(__VA_ARGS__)

	#define APP_ERROR(...) 		::Steve::Logger::getAppLogger()->error(__VA_ARGS__)
	#define APP_WARN(...) 		::Steve::Logger::getAppLogger()->warn(__VA_ARGS__)
	#define APP_INFO(...) 		::Steve::Logger::getAppLogger()->info(__VA_ARGS__)
	#define APP_TRACE(...) 		::Steve::Logger::getAppLogger()->trace(__VA_ARGS__)

	// Assertions
	#define APP_ASSERT(x, msg) {if(!(x)) { APP_ERROR(msg); CH_DEBUGBREAK} }
	#define CORE_ASSERT(x, msg) {if(!(x)) { CORE_ERROR(msg); CH_DEBUGBREAK} }

	#define APP__STATIC_ASSERT(x) (static_assert(x))
	#define CORE_STATIC_ASSERT(x) (static_assert(x))
#else
	#define CORE_ERROR(...)
	#define CORE_WARN(...) 
	#define CORE_INFO(...) 
	#define CORE_TRACE(...)
				
	#define APP_ERROR(...) 
	#define APP_WARN(...)
	#define APP_INFO(...) 
	#define APP_TRACE(...) 

	#define APP_ASSERT(...)
	#define CORE_ASSERT(...)
#endif

#endif
