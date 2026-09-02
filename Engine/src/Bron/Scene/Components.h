#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Util/Util.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <entt/entity/registry.hpp>

#include <optional>
#include <string>
#include <vector>

#include "Bron/Graphics/Components/BufferExtentions.h"
#include "Bron/Graphics/MaterialBase.h"
#include "Bron/Graphics/VertexArray.h"

namespace Bron
{
	// --------------------------------------------------------------------
	// Tag
	// --------------------------------------------------------------------

	struct TagComponent
	{
		std::string name = "Default name";

		TagComponent() = default;
		explicit TagComponent(std::string n) : name(std::move(n)) {}
	};


	// --------------------------------------------------------------------
	// Transform
	// --------------------------------------------------------------------

	struct TransformComponent
	{
		glm::mat4& GetMatrix()
		{
			if (IsDirty())
			{
				OPosition = Position;
				ORotationQuat = RotationQuat;
				OScaling = Scaling;

				glm::mat4 rotation = glm::toMat4(glm::quat(RotationQuat));

				Matrix = glm::translate(glm::mat4(1.0f), Position)
					* rotation
					* glm::scale(glm::mat4(1.0f), Scaling);
			}
			return Matrix;
		}

		bool IsDirty() const
		{
			BR_PROFILE_FUNCTION();
			return !(compare_floats_bits(Position, OPosition)
				&& compare_floats_bits((glm::vec4*)(&RotationQuat), (glm::vec4*)(&ORotationQuat))
				&& compare_floats_bits(Scaling, OScaling));
		}

		TransformComponent() : Position(0.0),
			RotationQuat({1.0f, 0.0f, 0.0f, 0.0f}), Scaling(1.0), Matrix(1.0f),
			OPosition(0.0), ORotationQuat({1.0f, 0.0f, 0.0f, 0.0f}), OScaling(1.0) {}

		operator glm::mat4&() { return GetMatrix(); }
		glm::mat4& operator*() { return GetMatrix(); }

		glm::vec3 Position;
		glm::quat RotationQuat; // w,x,y,z
		glm::vec3 Scaling;

	private:
		glm::mat4 Matrix;

		glm::vec3 OPosition;
		glm::quat ORotationQuat;
		glm::vec3 OScaling;
	};


	// --------------------------------------------------------------------
	// Hierarchy
	// --------------------------------------------------------------------

	struct HierarchyComponent
	{
		entt::entity parent = entt::null;
		std::vector<entt::entity> children;
	};


	// --------------------------------------------------------------------
	// Mesh
	// --------------------------------------------------------------------

	enum VertexVariables
	{
		POSITIONS,
		NORMALS,
		UVS,
		TANGENTS
	};

	struct MeshData
	{
		std::vector<glm::vec3> positions;
		std::vector<u32> indices;
		std::optional<std::vector<glm::vec3>> normals;
		std::optional<std::vector<glm::vec2>> uvs;
		std::optional<std::vector<glm::vec3>> tangents;
	};

	struct MeshComponent
	{
		MeshData vertexData;
		Ref<MaterialBase> material;

		// Lazily built by GetVao(); not part of the mesh's data.
		Ref<VertexArray> vao = nullptr;
	};

	// Average of the given vertices.
	glm::vec3 FindCentroid(const glm::vec3* vertices, uint64_t n);

	// Returns the mesh's vertex array, building it against the given layout on first use.
	Ref<VertexArray> GetVao(MeshComponent& mesh, const NamedBufferLayout<VertexVariables>& bufferLayout);


	// --------------------------------------------------------------------
	// Point light
	// --------------------------------------------------------------------

	struct PointLightComponent
	{
		glm::vec3 color{1.0f};

		PointLightComponent() = default;
		explicit PointLightComponent(const glm::vec3& c) : color(c) {}
	};
}
