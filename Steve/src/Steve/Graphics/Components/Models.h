#ifndef __MODELS_HADER__
#define __MODELS_HADER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "Steve/Graphics/Texture.h"
#include "Steve/Graphics/Buffer.h"
#include "Steve/Graphics/VertexArray.h"
#include "Steve/Graphics/Shader.h"

#include <map>

#include "MiscellaneousComponents.h"
#include "Model.h"
#include "Uniforms.h"
#include "Steve/ECS/Entity.h"
#include "Steve/Graphics/Camera.h"
#include "Steve/Graphics/Renderer/2D.h"

namespace Steve
{
	struct Cube : public Model
	{
		virtual glm::vec3 GetColor() const;
		virtual void SetColor(glm::vec3 color);

	protected:
		Cube(RegistryData* reg)
			: Model(reg) {}
	};


	enum class LightDataTypes
	{
		Position,
		Color
	};


#define LightData UniformData<LightDataTypes>
	struct PointLight : public Cube
	{
		// Override because the lightdata component also has to be changed.
		void SetColor(glm::vec3 color) override;
		void SetUniformPosition(glm::vec3 pos);

		PointLight(RegistryData* reg, LightData&& context);
	};

	// Returns vertices, normals, indices
	// Delete only vertices after usage.
	std::tuple<glm::vec3*, glm::vec3*, u32*> GenCubeSmoothVertices(glm::vec3 dimensions);

	// Returns vertices, normals, indices, #vertices/normals, #indices
	// Delete only vertices after usage.
	std::tuple<glm::vec3*, glm::vec3*, u32*, u32, u32> GenSphereSmoothVertices(glm::vec3 position, float radius, uint32_t accuracy);
}

#endif