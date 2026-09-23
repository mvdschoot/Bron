//
// Created by mathi on 22-9-2026.
//


#pragma once
#include "BufferExtentions.h"
#include "Bron/Core/Core.h"
#include "glm/glm.hpp"


#include <optional>
#include <vector>

namespace bron {
class VertexArray;

enum VertexVariables { kPositions, kNormals, kUvs, kTangents, kSmoothNormals };

struct AABB {
	AABB() : min(0.0f), max(0.0f) {}
	AABB(glm::vec3 a, glm::vec3 b);

	glm::vec3 min;
	glm::vec3 max;
};

struct MeshData {
	std::vector<glm::vec3> positions;
	std::vector<u32> indices;
	std::optional<std::vector<glm::vec3>> normals;
	std::optional<std::vector<glm::vec2>> uvs;
	std::optional<std::vector<glm::vec3>> tangents;

	// Normals with the hard edges welded shut - see SmoothNormals(). Derived from the
	// positions and indices, so it is filled in on demand rather than by the importer.
	std::optional<std::vector<glm::vec3>> smooth_normals;
};

// The one vertex format every mesh is uploaded in. It belongs to the mesh, not to a
// shader: attribute locations follow the order below, and a shader reads whichever of
// them it needs by declaring the same 'layout(location = N)'. The outline shader, for
// one, reads positions and smooth normals out of the same vertex array Phong draws with.
inline const auto kDefaultVertexLayout = NamedBufferLayout<VertexVariables>(
		{{VertexVariables::kPositions, {"a_Position", ShaderDataType::kFloat3}},
		 {VertexVariables::kNormals, {"a_Normal", ShaderDataType::kFloat3}},
		 {VertexVariables::kUvs, {"a_TexCoords", ShaderDataType::kFloat2}},
		 {VertexVariables::kSmoothNormals, {"a_SmoothNormal", ShaderDataType::kFloat3}}});

// Uploads the mesh in kDefaultVertexLayout. Attributes the mesh lacks are filled in on
// 'mesh_data' first, so any mesh fits the format.
Ref<VertexArray> GenerateVao(MeshData& mesh_data);
std::vector<glm::vec3> SmoothNormals(const MeshData& mesh);
glm::vec3 FindCentroid(const glm::vec3* vertices, const uint64_t n);
AABB GenerateAABB(const MeshData& mesh_data);

} // namespace bron
