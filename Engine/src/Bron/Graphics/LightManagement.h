//
// Created by mathijs on 8/3/25.
//

#pragma once

#include <entt/entity/registry.hpp>

#include <glm/glm.hpp>

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Graphics/Buffer.h"

#define BR_POINTLIGHT_UBO_INDEX 0

#define BR_POINTLIGHT_MAX 32

namespace bron {

class Scene;

class LightManagement {
public:
	struct ShaderPointlightStruct {
		glm::vec3 position{0.0f};
		float padding1;

		glm::vec3 color{1.0f};
		float padding2;
	};

	// Holds the scene rather than just its registry: a light's position has to be resolved through
	// the hierarchy, which only the scene can do.
	explicit LightManagement(Scene& scene) : scene_(scene) {}
	~LightManagement() = default;

	// Uploads every point light and binds the UBO. Call once per frame, before the draw loop.
	// The buffer is a fixed BR_POINTLIGHT_MAX entries, so re-uploading it unconditionally is cheaper
	// than tracking what changed.
	void Bind();

	// Clamped to BR_POINTLIGHT_MAX: the shader indexes a fixed size array with this count, and Bind()
	// only fills that many slots.
	[[nodiscard]] u8 NumberPointLights() const;

private:
	Scene& scene_;

	Ref<UniformBuffer> ubo_;
};

} // namespace bron
