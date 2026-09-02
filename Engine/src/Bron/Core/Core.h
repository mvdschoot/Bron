#pragma once

#include <cstdint>
#include <memory>

#include "Config.h"

#define PI 3.14159265358979323846
#define BIT(x) (1 << x)

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define usize uint64_t

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t
#define isize int64_t


#define GLFW_INCLUDE_NONE

#define BR_EXPAND_MACRO(x) x
#define BR_STRINGIFY_MACRO(x) #x

#define FOLD_LAMBDA_WITH_REFERENCE(expr) ([&, this]() expr, ...);


#if defined(_WIN32)
	#define BR_PLATFORM_WINDOWS
#elif defined(__APPLE__)
	#define BR_PLATFORM_MACOS
#elif defined(__linux__)
	#define BR_PLATFORM_LINUX
#else
	#define BR_PLATFORM_UNKNOWN
#endif

#if defined(BR_PLATFORM_WINDOWS)
	#define DLL_EXPORT __declspec(dllexport)
	#define DLL_IMPORT __declspec(dllimport)
#elif defined(__GNUC__) || defined(__clang__)
	#define DLL_EXPORT __attribute__((visibility("default")))
	#define DLL_IMPORT
#else
	// Unknown dynamic-link semantics; static linking still works.
	#define DLL_EXPORT
	#define DLL_IMPORT
#endif

#if defined(BR_DEBUG)
	#if defined(_MSC_VER)
		#define BR_DEBUGBREAK __debugbreak();
	#elif defined(__GNUC__) || defined(__clang__)
		#include <csignal>
		#define BR_DEBUGBREAK raise(SIGTRAP);
	#else
		#define BR_DEBUGBREAK
	#endif
#else
	#define BR_DEBUGBREAK
#endif

// Bron is a static library by default, where dllexport/dllimport are both
// wrong. BR_SHARED is defined by CMake only for a shared build.
#if defined(BR_SHARED)
	#if defined(BR_COMPILE)
		#define BR_API DLL_EXPORT
	#else
		#define BR_API DLL_IMPORT
	#endif
#else
	#define BR_API
#endif

namespace Bron
{
	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename ... Args>
	constexpr Ref<T> createRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T, typename ... Args>
	constexpr Scope<T> createScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}


	// Tuple folding stuff
	template <std::size_t... Idx>
	auto make_index_dispatcher(std::index_sequence<Idx...>) {
		return [](auto&& f) { (f(std::integral_constant<std::size_t, Idx>{}), ...); };
	}

	template <std::size_t N>
	auto make_index_dispatcher() {
		return make_index_dispatcher(std::make_index_sequence<N>{});
	}

	template <typename Tuple, typename Func>
	void for_each(Tuple&& t, Func&& f) {
		constexpr auto n = std::tuple_size_v<std::decay_t<Tuple>>;
		auto dispatcher = make_index_dispatcher<n>();
		dispatcher([&f, &t](auto idx) { f(std::get<idx>(std::forward<Tuple>(t))); });
	}

	template<typename Tuple, typename F>
	void ApplyToTuple(const Tuple& t, F func) {
		std::apply([&](const auto&... args) {
			(func(args), ...);
		}, t);
	}
}
