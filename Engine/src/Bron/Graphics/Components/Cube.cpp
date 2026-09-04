//
// Created by mathijs on 8/2/25.
//

#include "Cube.h"

#include "Bron/Graphics/Phong/PhongMaterial.h"
#include "Bron/Scene/Scene.h"

namespace bron {
	entt::entity CreateCube(Scene &target, const Ref<MaterialBase> &material) {
		MeshData mesh_data;

		// Cube vertices (positions)
		mesh_data.positions = {
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
		mesh_data.indices = {
			0, 1, 2, 2, 3, 0, // back
			4, 5, 6, 6, 7, 4, // front
			0, 4, 7, 7, 3, 0, // left
			1, 5, 6, 6, 2, 1, // right
			3, 2, 6, 6, 7, 3, // top
			0, 1, 5, 5, 4, 0  // bottom
		};

		// Optional normals (flat shading per face)
		mesh_data.normals = std::vector<glm::vec3>{
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
		mesh_data.uvs = std::vector<glm::vec2>{
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
		mesh_data.tangents = std::vector<glm::vec3>{
		        { 1.0f, 0.0f, 0.0f}, // 0
				{ 1.0f, 0.0f, 0.0f}, // 1
				{ 1.0f, 0.0f, 0.0f}, // 2
				{ 1.0f, 0.0f, 0.0f}, // 3
				{ 1.0f, 0.0f, 0.0f}, // 4
				{ 1.0f, 0.0f, 0.0f}, // 5
				{ 1.0f, 0.0f, 0.0f}, // 6
				{ 1.0f, 0.0f, 0.0f}  // 7
		};

		const entt::entity cube = target.CreateEntity("Cube");
		target.reg.emplace<MeshComponent>(cube, std::move(mesh_data), material);

		return cube;
	}

	entt::entity CreatePhongCube(Scene &target, const glm::vec3 color) {
		const Ref<PhongMaterial> material = CreateRef<PhongMaterial>();
		material->Set(PhongMaterialVariables::kDiffuse, color);
		material->Set(PhongMaterialVariables::kSpecular, color);
		material->Set(PhongMaterialVariables::kShininess, 5.0f);
		material->Set(PhongMaterialVariables::kShininessStrength, 1.0f);
		material->Set(PhongMaterialVariables::kAmbientFactor, 0.2f);

		return CreateCube(target, material);
	}
} // Bron
