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

	// Regenerates the UBO when any light moved or changed, then binds it.
	void bind();

	// Forces a UBO regeneration on the next bind(). Call after adding a light or editing its color,
	// neither of which the transform dirty check can see.
	void MarkDirty() { isDirty = true; }

	[[nodiscard]] u8 numberPointLights() const;

private:
	void generateUbo();

	entt::registry& reg;

	Ref<UniformBuffer> pointlightsUbo;

	bool isDirty = true;
};

} // Steve

#endif //LIGHTMANAGEMENT_H
