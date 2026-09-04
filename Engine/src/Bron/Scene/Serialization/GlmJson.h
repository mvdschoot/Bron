#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <nlohmann/json.hpp>

// glm's types are plain structs, so nlohmann has no way to read or write them
// on its own. These live in namespace glm so ADL finds them wherever a
// component serializes a vector, quaternion or matrix.
namespace glm {
template<length_t L, typename T, qualifier Q>
void to_json(nlohmann::json& j, const vec<L, T, Q>& v) {
	j = nlohmann::json::array();
	for (length_t i = 0; i < L; ++i)
		j.push_back(v[i]);
}

template<length_t L, typename T, qualifier Q>
void from_json(const nlohmann::json& j, vec<L, T, Q>& v) {
	for (length_t i = 0; i < L; ++i)
		j.at(i).get_to(v[i]);
}

// w,x,y,z, matching the order the engine stores rotations in.
template<typename T, qualifier Q>
void to_json(nlohmann::json& j, const qua<T, Q>& q) {
	j = nlohmann::json::array({q.w, q.x, q.y, q.z});
}

template<typename T, qualifier Q>
void from_json(const nlohmann::json& j, qua<T, Q>& q) {
	j.at(0).get_to(q.w);
	j.at(1).get_to(q.x);
	j.at(2).get_to(q.y);
	j.at(3).get_to(q.z);
}

// Column major, so a mat4 becomes an array of four columns.
template<length_t C, length_t R, typename T, qualifier Q>
void to_json(nlohmann::json& j, const mat<C, R, T, Q>& m) {
	j = nlohmann::json::array();
	for (length_t i = 0; i < C; ++i)
		j.push_back(m[i]);
}

template<length_t C, length_t R, typename T, qualifier Q>
void from_json(const nlohmann::json& j, mat<C, R, T, Q>& m) {
	for (length_t i = 0; i < C; ++i)
		j.at(i).get_to(m[i]);
}
} // namespace glm
