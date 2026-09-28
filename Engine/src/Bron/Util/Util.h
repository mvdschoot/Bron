#pragma once

#include <glm/glm.hpp>
#include <string>

#include "Bron/Core/Core.h"

#include <optional>
#include <type_traits>

namespace bron {
/// Whether 'T' is one of the types in a type list such as ComponentList<A, B, C>. Works on any
/// list-like template, so it does not need to know the list type.
template<typename T, typename List>
struct IsInList;

template<typename T, template<typename...> typename List, typename... Ts>
struct IsInList<T, List<Ts...>> : std::disjunction<std::is_same<T, Ts>...> {};

template<typename T, typename List>
concept InList = IsInList<T, List>::value;

/// Whether 'T' is 'Template' filled in with some arguments, e.g. ComponentList<A, B> for
/// ComponentList.
template<typename T, template<typename...> typename Template>
struct IsSpecializationOf : std::false_type {};

template<template<typename...> typename Template, typename... Ts>
struct IsSpecializationOf<Template<Ts...>, Template> : std::true_type {};

template<typename T, template<typename...> typename Template>
concept SpecializationOf = IsSpecializationOf<T, Template>::value;

bool CompareFloat(float x, float y, float epsilon = 0.01f);
bool CompareFloats(glm::vec3 a, glm::vec3 b, float epsilon = 0.01f);

bool CompareFloatBits(float x, float y);
bool CompareFloatsBits(const glm::vec2& a, const glm::vec2& b);
bool CompareFloatsBits(const glm::vec3& a, const glm::vec3& b);
bool CompareFloatsBits(glm::vec3* a, glm::vec3* b);
bool CompareFloatsBits(glm::vec4* a, glm::vec4* b);

std::string PrintMatrix(glm::mat4& matrix);

std::tuple<glm::vec3*, glm::vec3*, uint32_t*, u32, u32> GenSphereSmoothVertices(glm::vec3 position, float radius,
																				u32 accuracy);

std::string ToLowerCase(const std::string& str);

template<std::ranges::range T, typename Pred>
auto* Find(T& container, Pred&& predicate) {
	for (auto& item: container) {
		if (predicate(item)) {
			return &item;
		}
	}
	return static_cast<std::ranges::range_value_t<T>*>(nullptr);
}

template<std::ranges::input_range R, typename F>
auto Map(R&& range, F&& func) {
	using Result = std::invoke_result_t<F&, std::ranges::range_reference_t<R>>;

	std::vector<Result> result;

	for (auto&& x: range)
		result.push_back(std::invoke(func, x));

	return result;
}

} // namespace bron
