//
// Created by mathijs on 8/3/25.
//

#ifndef LIGHTMANAGEMENT_H
#define LIGHTMANAGEMENT_H

#include <entt/entity/registry.hpp>

#include <glm/glm.hpp>

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"
#include "Steve/Graphics/Buffer.h"

#define POINTLIGHT_UBO_INDEX 0

#define POINTLIGHT_MAX 32

namespace Steve {

class LightManagement {
public:
	struct ShaderPointlightStruct {
		glm::vec3 position{0.0f};
		float padding1;

		glm::vec3 color{1.0f};
		float padding2;
	};

	explicit LightManagement(entt::registry& reg) : reg(reg) {}
	~LightManagement() = default;

	// Uploads every point light and binds the UBO. Call once per frame, before the draw loop.
	// The buffer is a fixed POINTLIGHT_MAX entries, so re-uploading it unconditionally is cheaper
	// than tracking what changed.
	void bind();

	// Clamped to POINTLIGHT_MAX: the shader indexes a fixed size array with this count, and bind()
	// only fills that many slots.
	[[nodiscard]] u8 numberPointLights() const;

private:
	entt::registry& reg;

	Ref<UniformBuffer> ubo;
};

} // Steve

#endif //LIGHTMANAGEMENT_H
