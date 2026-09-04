#pragma once

#include <cstdint>
#include <memory>

#include "Config.h"

#define BR_BIT(x) (1 << (x))

#define BR_EXPAND_MACRO(x) x
#define BR_STRINGIFY_MACRO(x) #x

#define BR_FOLD_LAMBDA_WITH_REFERENCE(expr) ([&, this]() expr, ...);

#define GLFW_INCLUDE_NONE

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
	#define BR_DLL_EXPORT __declspec(dllexport)
	#define BR_DLL_IMPORT __declspec(dllimport)
#elif defined(__GNUC__) || defined(__clang__)
	#define BR_DLL_EXPORT __attribute__((visibility("default")))
	#define BR_DLL_IMPORT
#else
	// Unknown dynamic-link semantics; static linking still works.
	#define BR_DLL_EXPORT
	#define BR_DLL_IMPORT
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
		#define BR_API BR_DLL_EXPORT
	#else
		#define BR_API BR_DLL_IMPORT
	#endif
#else
	#define BR_API
#endif

namespace bron {

// Short integer names used throughout the engine. These are type aliases rather
// than macros so they obey scope and cannot rewrite tokens inside third-party
// headers.
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using usize = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using isize = std::int64_t;

inline constexpr double kPi = 3.14159265358979323846;

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename... Args>
constexpr Ref<T> CreateRef(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T>
using Scope = std::unique_ptr<T>;

template <typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

// Tuple folding helpers.
template <std::size_t... Idx>
auto MakeIndexDispatcher(std::index_sequence<Idx...>)
{
	return [](auto&& f) { (f(std::integral_constant<std::size_t, Idx>{}), ...); };
}

template <std::size_t N>
auto MakeIndexDispatcher()
{
	return MakeIndexDispatcher(std::make_index_sequence<N>{});
}

template <typename Tuple, typename Func>
void ForEachInTuple(Tuple&& t, Func&& f)
{
	constexpr auto n = std::tuple_size_v<std::decay_t<Tuple>>;
	auto dispatcher = MakeIndexDispatcher<n>();
	dispatcher([&f, &t](auto idx) { f(std::get<idx>(std::forward<Tuple>(t))); });
}

template <typename Tuple, typename F>
void ApplyToTuple(const Tuple& t, F func)
{
	std::apply([&](const auto&... args) { (func(args), ...); }, t);
}

}  // namespace bron
