//
// Created by mathijs on 8/3/25.
//

#include "LightManagement.h"

#include <algorithm>
#include <vector>

#include "Bron/Scene/Components.h"
#include "Bron/Scene/Scene.h"

namespace bron {
	u8 LightManagement::NumberPointLights() const {
		return static_cast<u8>(std::min<usize>(scene.reg.view<PointLightComponent>().size(), POINTLIGHT_MAX));
	}

	void LightManagement::Bind() {
		BR_PROFILE_FUNCTION();

		// Zero filled, so slots past the last light read as black lights at the origin.
		std::vector<ShaderPointlightStruct> lights(POINTLIGHT_MAX);

		usize i = 0;
		for (auto [entity, transform, light] : scene.reg.view<TransformComponent, PointLightComponent>().each()) {
			if (i >= POINTLIGHT_MAX) {
				BR_CORE_WARN("Scene has more than {} point lights; the rest are ignored.", POINTLIGHT_MAX);
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
			ubo->SetData(reinterpret_cast<const u8*>(lights.data()), totalSize);

		ubo->Bind(POINTLIGHT_UBO_INDEX);
	}
} // Bron
