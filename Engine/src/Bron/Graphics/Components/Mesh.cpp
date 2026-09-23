//
// Created by mathi on 22-9-2026.
//

#include "Mesh.h"

#include "Bron/Graphics/VertexArray.h"

namespace bron {

AABB::AABB(glm::vec3 a, glm::vec3 b) {
	min = glm::min(a, b);
	max = glm::max(a, b);
}

Ref<VertexArray> GenerateVao(MeshData& mesh_data) {
	const NamedBufferLayout<VertexVariables>& buffer_layout = kDefaultVertexLayout;

	// MultipleNamedBufferData will place the data at the correct places.
	MultipleNamedBufferData<VertexVariables> vertex_buffers(&buffer_layout, mesh_data.positions.size());

	// Set the data. The MultipleNamedBufferData objects will take care of placement of the data.
	for (const VertexVariables variable: buffer_layout.keys) {
		switch (variable) {
			case kPositions:
				vertex_buffers.Set(VertexVariables::kPositions, reinterpret_cast<u8*>(&mesh_data.positions[0]));
				break;

			case kNormals:
				// Without authored normals, the welded ones are the best guess at the surface.
				if (!mesh_data.normals.has_value())
					mesh_data.normals = SmoothNormals(mesh_data);

				vertex_buffers.Set(VertexVariables::kNormals, reinterpret_cast<u8*>(&mesh_data.normals.value()[0]));
				break;

			case kUvs:
				// A mesh without UVs samples no texture in a meaningful way, so any constant will do.
				if (!mesh_data.uvs.has_value())
					mesh_data.uvs = std::vector<glm::vec2>(mesh_data.positions.size(), glm::vec2(0.0f));

				vertex_buffers.Set(VertexVariables::kUvs, reinterpret_cast<u8*>(&mesh_data.uvs.value()[0]));
				break;

			case kSmoothNormals:
				if (!mesh_data.smooth_normals.has_value())
					mesh_data.smooth_normals = SmoothNormals(mesh_data);

				vertex_buffers.Set(VertexVariables::kSmoothNormals,
								   reinterpret_cast<u8*>(&mesh_data.smooth_normals.value()[0]));
				break;

			case kTangents:
				BR_CORE_ASSERT(mesh_data.tangents.has_value(),
							   "Shader requires the tangents of the vertices, but they have not been given");
				vertex_buffers.Set(VertexVariables::kTangents, reinterpret_cast<u8*>(&mesh_data.tangents.value()[0]));
				break;

			default:
				BR_CORE_ASSERT(false, "Unknown vertex variable type provided to vertexBuffers.Set()");
				break;
		}
	}

	Ref<VertexArray> vao = VertexArray::Create();

	// Add the vertex buffer to the vao
	const Ref<VertexBuffer> b =
			VertexBuffer::Create(reinterpret_cast<float*>(vertex_buffers.data), vertex_buffers.GetTotalSize());
	b->SetBufferLayout(buffer_layout);
	vao->AddVertexBuffer(b);

	// Add the index buffer to the vao
	const Ref<IndexBuffer> index_buffer = IndexBuffer::Create(&mesh_data.indices[0], mesh_data.indices.size());
	vao->SetIndexBuffer(index_buffer);

	return vao;
}

// A mesh gets its hard edges from vertices that share a position but carry different
// normals - a cube corner is three vertices, one per face. That is what shading wants, but
// an outline pass that pushes each vertex along its own normal tears those corners apart
// and leaves a gap. These are the same normals averaged over every vertex at a position,
// so the extruded copy stays welded together.
std::vector<glm::vec3> SmoothNormals(const MeshData& mesh) {
	// Positions are matched bit for bit. An importer that splits a vertex to give it a
	// second normal copies the position across untouched, which is exactly the case that
	// has to be merged here; positions that merely happen to be close were separate
	// vertices to begin with and should stay that way.
	struct PositionHash {
		usize operator()(const glm::vec3& position) const {
			const std::hash<float> hash;
			return hash(position.x) ^ hash(position.y) << 1 ^ hash(position.z) << 2;
		}
	};

	std::unordered_map<glm::vec3, glm::vec3, PositionHash> welded;
	welded.reserve(mesh.positions.size());

	// glm's default constructor leaves a vector uninitialised, so the running total is
	// seeded explicitly rather than through operator[].
	auto accumulate = [&welded](const glm::vec3& position, const glm::vec3& face_normal) {
		welded.try_emplace(position, glm::vec3(0.0f)).first->second += face_normal;
	};

	// The cross product of two edges points along the face normal and scales with the
	// triangle's area, so accumulating it unnormalised weights each face by its size. A
	// sliver of a triangle then sways the average far less than a large neighbour does.
	for (usize i = 0; i + 2 < mesh.indices.size(); i += 3) {
		const glm::vec3& a = mesh.positions[mesh.indices[i]];
		const glm::vec3& b = mesh.positions[mesh.indices[i + 1]];
		const glm::vec3& c = mesh.positions[mesh.indices[i + 2]];

		const glm::vec3 face_normal = glm::cross(b - a, c - a);

		accumulate(a, face_normal);
		accumulate(b, face_normal);
		accumulate(c, face_normal);
	}

	std::vector<glm::vec3> smooth_normals(mesh.positions.size());
	for (usize i = 0; i < mesh.positions.size(); i++) {
		const auto entry = welded.find(mesh.positions[i]);

		const glm::vec3 accumulated = entry != welded.end() ? entry->second : glm::vec3(0.0f);
		const float length = glm::length(accumulated);

		// Degenerate or unreferenced vertices cancel out to nothing. Falling back on the
		// shading normal keeps them moving with the rest of the surface; with no normals
		// at all the vertex simply stays put.
		if (length > 0.0f)
			smooth_normals[i] = accumulated / length;
		else if (mesh.normals.has_value())
			smooth_normals[i] = mesh.normals.value()[i];
		else
			smooth_normals[i] = glm::vec3(0.0f);
	}

	return smooth_normals;
}

glm::vec3 FindCentroid(const glm::vec3* vertices, const uint64_t n) {
	glm::vec3 res(0.0);

	for (usize i = 0; i < n; i++) {
		res += vertices[i];
	}

	return res / static_cast<float>(n);
}

AABB GenerateAABB(const MeshData& mesh_data) {
	glm::vec3 min(std::numeric_limits<float>::max());
	glm::vec3 max(std::numeric_limits<float>::lowest());

	for (const glm::vec3& p: mesh_data.positions) {
		min = glm::min(min, p);
		max = glm::max(max, p);
	}

	return {min, max};
}


} // namespace bron
