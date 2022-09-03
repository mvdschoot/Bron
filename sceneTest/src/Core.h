#ifndef CORE_HEADER
#define CORE_HEADER

#include <cstdint>
#include <memory>

#define GLFW_INCLUDE_NONE

#define CH_EXPAND_MACRO(x) x
#define CH_STRINGIFY_MACRO(x) #x

#define std_int uint32_t

#if defined(_MSC_VER)
	#define CH_PLATFORM_WINDOWS
#elif defined(__GNUC__)
	#define CH_PLATFORM_LINUX
#else
	#define CH_PLATFORM_UNKNOWN
#endif

#ifdef CH_PLATFORM_WINDOWS
    #define DLL_EXPORT __declspec(dllexport)
    #define DLL_IMPORT __declspec(dllimport)
#elif defined(CH_PLATFORM_LINUX)
    #define DLL_EXPORT __attribute__((visibility("default")))
    #define DLL_IMPORT
#else
	#define DLL_EXPORT
	#define DLL_IMPORT
	#pragma warning Unknown dynamic link import/export semantics.
#endif

#if defined(CH_DEBUG)
	#ifdef CH_PLATFORM_WINDOWS
		#define CH_DEBUGBREAK __debugbreak;
	#elif defined(CH_PLATFORM_LINUX)
		#include <signal.h>
		#define CH_DEBUGBREAK raise(SIGTRAP);
	#else
		#define CH_DEBUGBREAK
	#endif
#endif

#if CH_COMPILE
#   define CHEETS_API DLL_EXPORT
#else
#   define CHEETS_API DLL_IMPORT
#endif

namespace Cheets {
	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> createRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> createScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<class I>
	struct RemoveSP{
		typedef I type;
	};

	template<class I>
	struct RemoveSP<std::shared_ptr<I>>
	{
		typedef typename RemoveSP<I>::type type;
	};

}

#endif