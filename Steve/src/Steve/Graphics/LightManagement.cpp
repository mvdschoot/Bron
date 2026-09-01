//
// Created by mathijs on 8/3/25.
//

#include "LightManagement.h"

#include "Steve/Scene/Components.h"

namespace Steve {
	u8 LightManagement::numberPointLights() const {
		return static_cast<u8>(reg.view<PointLightComponent>().size());
	}

	void LightManagement::bind() {
		CH_PROFILE_FUNCTION();

		// Lights added or removed since the last build, e.g. from the editor's Add Component menu.
		const usize lightCount = reg.view<PointLightComponent>().size();
		isDirty |= lightCount != lastLightCount;

		// A light that moved is only visible through its transform, so scan for that too. Colour edits
		// are the one change neither check can see, and set the flag through MarkDirty().
		for (auto [entity, transform, light] : reg.view<TransformComponent, PointLightComponent>().each()) {
			isDirty |= transform.IsDirty();

			// Lights are not drawn, so nothing else consumes their dirty state. Recompute the matrix
			// here to clear it, otherwise every frame would look dirty and rebuild the UBO.
			transform.GetMatrix();
		}

		if (isDirty || pointlightsUbo == nullptr) {
			generateUbo();
			isDirty = false;
			lastLightCount = lightCount;
		}

		if (!pointlightsUbo->isBound()) {
			pointlightsUbo->bind(POINTLIGHT_UBO_INDEX);
		}
	}

	void LightManagement::generateUbo() {
		CH_PROFILE_FUNCTION();

		constexpr usize lightDataSize = sizeof(ShaderPointlightStruct);
		constexpr usize totalSize = lightDataSize * POINTLIGHT_MAX;

		// Zero-filled, so unused slots read as black lights at the origin.
		std::vector<ShaderPointlightStruct> lights(POINTLIGHT_MAX);

		usize i = 0;
		for (auto [entity, transform, light] : reg.view<TransformComponent, PointLightComponent>().each()) {
			if (i >= POINTLIGHT_MAX) {
				CORE_WARN("Scene has more than {} point lights; the rest are ignored.", POINTLIGHT_MAX);
				break;
			}

			lights[i].position = transform.Position;
			lights[i].color = light.color;
			i++;
		}

		pointlightsUbo = UniformBuffer::Create(lights.data(), totalSize, POINTLIGHT_UBO_INDEX);
	}
} // Steve
