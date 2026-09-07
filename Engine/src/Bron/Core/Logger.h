#pragma once

#include <memory>
#include <array>
#include <cstdarg>
#include <cstddef>
#include <filesystem>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>

#include <magic_enum/magic_enum.hpp>

#include "Bron/Core/Core.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
// Lets containers of loggable structs be logged as-is: BR_CORE_INFO("{}", entries).
#include "spdlog/fmt/ranges.h"

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

} // namespace bron


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
	#define BR_APP_ASSERT(x, ...)                                                                                      \
		do {                                                                                                           \
			if (!(x)) {                                                                                                \
				BR_APP_ERROR(__VA_ARGS__);                                                                             \
				BR_DEBUGBREAK                                                                                          \
			}                                                                                                          \
		} while (0)
	#define BR_CORE_ASSERT(x, ...)                                                                                     \
		do {                                                                                                           \
			if (!(x)) {                                                                                                \
				BR_CORE_ERROR(__VA_ARGS__);                                                                            \
				BR_DEBUGBREAK                                                                                          \
			}                                                                                                          \
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
	#define BR_APP_ASSERT(x, ...)                                                                                      \
		do {                                                                                                           \
			(void) sizeof(!(x));                                                                                       \
		} while (0)
	#define BR_CORE_ASSERT(x, ...)                                                                                     \
		do {                                                                                                           \
			(void) sizeof(!(x));                                                                                       \
		} while (0)
#endif

#define BR_APP_STATIC_ASSERT(x) static_assert(x)
#define BR_CORE_STATIC_ASSERT(x) static_assert(x)

namespace bron::logging {
/// Turns a field into something fmt can print: enums come out as their names instead of
/// their numbers, and paths without the quoting and doubled separators fmt would give
/// them. Everything else is passed straight through and formats as it always did.
template<typename T>
decltype(auto) LogValue(const T& value) {
	if constexpr (std::is_enum_v<T>)
		return magic_enum::enum_name(value);
	else if constexpr (std::is_same_v<T, std::filesystem::path>)
		return value.string();
	else
		return (value);
}

/// Renders "Type(id=7, name=seven)" from the field list BR_LOGGABLE stringified and the
/// values themselves. field_names is the argument list exactly as it was written, so it
/// is split back apart on the commas here - one name per value, in order.
template<typename... Ts>
std::string FormatFields(const std::string_view type_name, const std::string_view field_names, const Ts&... values) {
	const std::array<std::string, sizeof...(Ts)> formatted{fmt::format("{}", LogValue(values))...};

	std::string out(type_name);
	out += '(';

	std::size_t start = 0;
	for (std::size_t i = 0; i < formatted.size(); ++i) {
		std::size_t end = field_names.find(',', start);
		if (end == std::string_view::npos)
			end = field_names.size();

		// The preprocessor keeps whatever spacing the caller wrote between arguments.
		std::string_view name = field_names.substr(start, end - start);
		while (!name.empty() && name.front() == ' ')
			name.remove_prefix(1);
		while (!name.empty() && name.back() == ' ')
			name.remove_suffix(1);

		if (i > 0)
			out += ", ";
		out += name;
		out += '=';
		out += formatted[i];

		start = end + 1;
	}

	out += ')';
	return out;
}
} // namespace bron::logging

/// Makes a struct loggable by naming its fields:
///
///     struct MyStruct {
///         int id;
///         std::string name;
///
///         BR_LOGGABLE(MyStruct, id, name)
///     };
///
///     BR_CORE_INFO("loaded {}", my_struct);   // loaded MyStruct(id=7, name=seven)
///     std::cout << my_struct;
///
/// The fields are named unqualified because the macro expands inside a member function.
/// Any expression works there, not just a member: `size()`, `inner.id`, and so on - it
/// is labelled with the text as written. format_as is fmt's own hook, found by ADL, so
/// this works in any access section and nested loggable structs format through theirs.
#define BR_LOGGABLE(Type, ...)                                                                                         \
	std::string ToLogString() const { return ::bron::logging::FormatFields(#Type, #__VA_ARGS__, __VA_ARGS__); }           \
	friend std::string format_as(const Type& self) { return self.ToLogString(); }                                         \
	friend std::ostream& operator<<(std::ostream& os, const Type& self) { return os << self.ToLogString(); }
