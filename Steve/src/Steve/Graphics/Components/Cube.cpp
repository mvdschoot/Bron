//
// Created by mathijs on 8/2/25.
//

#include "Cube.h"

Steve::Ref<Steve::Mesh> Steve::Cube::generateMesh(RegistryData *reg) {
	MeshData meshData;

	// Cube vertices (positions)
	meshData.positions = {
		{-0.5f, -0.5f, -0.5f}, // 0
		{ 0.5f, -0.5f, -0.5f}, // 1
		{ 0.5f,  0.5f, -0.5f}, // 2
		{-0.5f,  0.5f, -0.5f}, // 3
		{-0.5f, -0.5f,  0.5f}, // 4
		{ 0.5f, -0.5f,  0.5f}, // 5
		{ 0.5f,  0.5f,  0.5f}, // 6
		{-0.5f,  0.5f,  0.5f}  // 7
	};

	// 12 triangles (36 indices)
	meshData.indices = {
		0, 1, 2, 2, 3, 0, // back
		4, 5, 6, 6, 7, 4, // front
		0, 4, 7, 7, 3, 0, // left
		1, 5, 6, 6, 2, 1, // right
		3, 2, 6, 6, 7, 3, // top
		0, 1, 5, 5, 4, 0  // bottom
	};

	// Optional normals (flat shading per face)
	meshData.normals = std::vector<glm::vec3>{
	        { 0.0f,  0.0f, -1.0f}, // back
			{ 0.0f,  0.0f, -1.0f},
			{ 0.0f,  0.0f, -1.0f},
			{ 0.0f,  0.0f, -1.0f},

			{ 0.0f,  0.0f,  1.0f}, // front
			{ 0.0f,  0.0f,  1.0f},
			{ 0.0f,  0.0f,  1.0f},
			{ 0.0f,  0.0f,  1.0f}
	};

	// Optional UVs (basic cube projection)
	meshData.uvs = std::vector<glm::vec2>{
	        {0.0f, 0.0f}, // 0
			{1.0f, 0.0f}, // 1
			{1.0f, 1.0f}, // 2
			{0.0f, 1.0f}, // 3
			{0.0f, 0.0f}, // 4
			{1.0f, 0.0f}, // 5
			{1.0f, 1.0f}, // 6
			{0.0f, 1.0f}  // 7
	};

	// Optional tangents (approximate for a cube)
	meshData.tangents = std::vector<glm::vec3>{
	        { 1.0f, 0.0f, 0.0f}, // 0
			{ 1.0f, 0.0f, 0.0f}, // 1
			{ 1.0f, 0.0f, 0.0f}, // 2
			{ 1.0f, 0.0f, 0.0f}, // 3
			{ 1.0f, 0.0f, 0.0f}, // 4
			{ 1.0f, 0.0f, 0.0f}, // 5
			{ 1.0f, 0.0f, 0.0f}, // 6
			{ 1.0f, 0.0f, 0.0f}  // 7
	};

	return createRef<Mesh>(reg, std::move(meshData), generateMaterial());
}
