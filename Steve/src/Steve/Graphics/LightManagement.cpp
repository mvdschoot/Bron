//
// Created by mathijs on 8/3/25.
//

#include "LightManagement.h"

#include <algorithm>
#include <vector>

#include "Steve/Scene/Components.h"
#include "Steve/Scene/Scene.h"

namespace Steve {
	u8 LightManagement::numberPointLights() const {
		return static_cast<u8>(std::min<usize>(scene.reg.view<PointLightComponent>().size(), POINTLIGHT_MAX));
	}

	void LightManagement::bind() {
		CH_PROFILE_FUNCTION();

		// Zero filled, so slots past the last light read as black lights at the origin.
		std::vector<ShaderPointlightStruct> lights(POINTLIGHT_MAX);

		usize i = 0;
		for (auto [entity, transform, light] : scene.reg.view<TransformComponent, PointLightComponent>().each()) {
			if (i >= POINTLIGHT_MAX) {
				CORE_WARN("Scene has more than {} point lights; the rest are ignored.", POINTLIGHT_MAX);
				break;
			}

			// The shader lights in world space, so a light parented to something has to be composed
			// with its parents rather than uploaded at its local position.
			lights[i].position = glm::vec3(scene.WorldTransform(entity)[3]);
			lights[i].color = light.color;
			i++;
		}

		constexpr usize totalSize = sizeof(ShaderPointlightStruct) * POINTLIGHT_MAX;

		if (ubo == nullptr)
			ubo = UniformBuffer::Create(lights.data(), totalSize, POINTLIGHT_UBO_INDEX);
		else
			ubo->setData(reinterpret_cast<const u8*>(lights.data()), totalSize);

		ubo->bind(POINTLIGHT_UBO_INDEX);
	}
} // Steve
