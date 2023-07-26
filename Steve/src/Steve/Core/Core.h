#ifndef CORE_HEADER
#define CORE_HEADER

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

#define CH_EXPAND_MACRO(x) x
#define CH_STRINGIFY_MACRO(x) #x

#define FOLD_LAMBDA_WITH_REFERENCE(expr) ([&, this]() expr, ...);

#define BIT_FLAG_ENUM(name, ...) \
    enum name { __VA_ARGS__ }; \
    inline std::string GET_ENUM_STRING(name value) \
    { \
        struct Entry { name value; const char* nameStr; }; \
        const Entry entries[] = { __VA_ARGS__, {name(0), "None"} }; \
        for (const auto& entry : entries) \
        { \
            if (entry.value == value) \
            { \
                os << entry.nameStr; \
                return os; \
            } \
        } \
        return os << static_cast<int>(value); \
    }

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
		#define CH_DEBUGBREAK __debugbreak();
	#elif defined(CH_PLATFORM_LINUX)
		#include <signal.h>
		#define CH_DEBUGBREAK raise(SIGTRAP);
	#else
		#define CH_DEBUGBREAK
	#endif
#endif

#if defined(CH_COMPILE)
	#define STEVE_API DLL_EXPORT
#else
	#define STEVE_API DLL_IMPORT
#endif

namespace Steve
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

#endif
